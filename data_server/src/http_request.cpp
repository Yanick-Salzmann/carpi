#include "data_server/http_request.hpp"
#include "data_server/http_response.hpp"
#include "camera_handler.hpp"
#include "cookie_helper.hpp"
#include "range_helper.hpp"

#include <filesystem>
#include <common_utils/string.hpp>
#include <include/cef_parser.h>
#include <sys/socket.h>
#include <uuid/uuid.h>
#include <iostream>

namespace carpi::data {
    LOGGER_IMPL(HttpRequest);

    HttpRequest::HttpRequest(const std::string &method, const std::string &path, const std::string &version, const std::multimap<std::string, std::string> &headers, int socket) {
        if (method != "GET") {
            log->warn("Invalid HTTP method {}, must be GET", method);
            HttpResponse{HttpStatusCode::INVALID_METHOD, "GET only"}.write_to_socket(socket);
            return;
        }

        if (version != "HTTP/1.1") {
            log->warn("Invalid HTTP version {}, must be 'HTTP/1.1'", version);
            HttpResponse{HttpStatusCode::NOT_SUPPORTED, "Only HTTP/1.1 is supported"}.write_to_socket(socket);
            return;
        }

        if (path.empty()) {
            log->warn("HTTP path is empty");
            HttpResponse{HttpStatusCode::NOT_FOUND, "File not found"}.write_to_socket(socket);
            return;
        }

        auto file_path = path;
        if (file_path[0] == '/') {
            file_path = file_path.substr(1);
        }

        switch (find_request_type(file_path, file_path)) {
            case RequestType::LOCAL_FILE: {
                process_local_file(file_path, socket);
                break;
            }

            case RequestType::CAMERA_STREAM: {
                process_camera_stream(file_path, headers, socket);
                break;
            }

            default: {
                HttpResponse{HttpStatusCode::BAD_REQUEST, "Unknown request type"}.write_to_socket(socket);
                break;
            }
        }
    }

    RequestType HttpRequest::find_request_type(const std::string &path, std::string &remainder) {
        std::string type_part{path};
        const auto slash = path.find('/');
        if (slash != std::string::npos) {
            type_part = path.substr(0, slash);
            remainder = path.substr(slash + 1);
        }

        if (type_part.empty()) {
            return RequestType::UNKNOWN;
        }

        type_part = utils::to_lower(type_part);
        if (type_part == "local") {
            return RequestType::LOCAL_FILE;
        } else if (type_part == "camera") {
            return RequestType::CAMERA_STREAM;
        } else {
            return RequestType::UNKNOWN;
        }
    }

    void HttpRequest::process_local_file(const std::string &path, int socket) {
        std::filesystem::path ui_root{"../../carpi_master/ui"};
        std::filesystem::path ui_file = canonical(absolute(ui_root / path));
        if (!exists(ui_file)) {
            log->warn("HTTP file not found: {}", ui_file.string());
            HttpResponse{HttpStatusCode::NOT_FOUND, "File not found"}.write_to_socket(socket);
            return;
        }

        const auto extension = ui_file.extension().string();
        std::string mime_type{"application/octet-stream"};
        if (!extension.empty()) {
            const auto cef_mime_type = CefGetMimeType(extension);
            if (!cef_mime_type.ToString().empty()) {
                mime_type = cef_mime_type;
            }
        }

        HttpResponse{HttpStatusCode::OK, "OK"}
                .add_header("Content-Type", mime_type)
                .respond_with_file(ui_file.string())
                .write_to_socket(socket);
    }

    void HttpRequest::process_camera_stream(const std::string &path, const std::multimap<std::string, std::string> &headers, int socket) {
        HttpResponse response{HttpStatusCode::OK, "OK"};
        response.add_header("Content-Type", "video/mp4");

        CookieHelper cookie_helper{headers};
        cookie_helper.print(log);

        Range range{headers.lower_bound("range")->second};
        const auto &ranges = range.ranges();
        if (ranges.empty()) {
            response.status(HttpStatusCode::BAD_REQUEST, "Missing Range")
                    .write_to_socket(socket);
            return;
        }

        if (ranges.size() > 1) {
            response.status(HttpStatusCode::BAD_REQUEST, "Only one Range supported")
                    .write_to_socket(socket);
            return;
        }

        const auto req_range = ranges[0];
        if (req_range.type == RangeType::SUFFIX) {
            response.status(HttpStatusCode::BAD_REQUEST, "Ranges from end are not supported")
                    .write_to_socket(socket);
            return;
        }

        std::size_t start, end;
        if (req_range.type == RangeType::OPEN_END) {
            start = req_range.start;
            end = std::numeric_limits<std::size_t>::max();
        } else if (req_range.type == RangeType::BOUNDED) {
            start = req_range.start;
            end = req_range.end;
        } else {
            response.status(HttpStatusCode::BAD_REQUEST, "Ranges from end are not supported")
                    .write_to_socket(socket);
            return;
        }

        auto is_first_request = !cookie_helper.has_cookie("camera_stream");
        std::string stream_id = cookie_helper.cookie("camera_stream");
        if (!is_first_request) {
            if (!sCameraHandler->is_current_stream(stream_id, start)) {
                sCameraHandler->cancel_stream(stream_id);
                is_first_request = true;
            }
        }

        if (is_first_request && stream_id.empty()) {
            stream_id = create_uuid();
        }

        if (is_first_request) {
            sCameraHandler->start_stream(stream_id);
            log->info("Started new stream");
        }


        std::mutex final_lock{};
        std::condition_variable final_var{};
        bool completed = false;

        if (!sCameraHandler->request_range(stream_id, start, end, [&final_var, &completed, socket, &response, start, stream_id](const std::vector<uint8_t> &data, std::size_t bytes) {
            response.status(HttpStatusCode::PARTIAL_CONTENT, "OK")
                    .add_header("Content-Range", fmt::format("bytes {}-{}/1000000000000", start, start + bytes - 1))
                    .add_header("Content-Length", fmt::format("{}", bytes))
                    .add_header("Set-Cookie", fmt::format("camera_stream={}", stream_id))
                    .write_to_socket(socket);
            send(socket, data.data(), bytes, 0);
            completed = true;
            final_var.notify_all();
            return true;
        })) {
            response.status(HttpStatusCode::RANGE_NOT_SATISFIABLE, "Invalid request range")
                    .write_to_socket(socket);
            return;
        }

        std::unique_lock<std::mutex> l{final_lock};
        final_var.wait(l, [&completed]() { return completed; });
    }

    std::string HttpRequest::create_uuid() {
        char uuid_str[37]{};
        uuid_t uuid{};
        uuid_generate_random(uuid);
        uuid_unparse_lower(uuid, uuid_str);
        return std::string{uuid_str, uuid_str + 36};
    }
}