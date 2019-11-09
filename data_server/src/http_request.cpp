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
        range.print(log);

        const auto is_first_request = !cookie_helper.has_cookie("camera_stream");
        if(!is_first_request) {
            const auto stream_id = cookie_helper.cookie("camera_stream");
        }

        if (!cookie_helper.has_cookie("camera_stream")) {

        }

        response.write_to_socket(socket);

        std::mutex final_lock{};
        std::condition_variable final_var{};
        auto completed = false;

        FILE* f = fopen("camera_out.mp4", "wb");

        sCameraHandler->begin_streaming([socket, &final_lock, &final_var, &completed, f](void *data, std::size_t size) {
//            std::stringstream hdr_stream{};
//            hdr_stream << std::hex << size << "\r\n";
//            const auto hdr_line = hdr_stream.str();
//            if(::send(socket, hdr_line.c_str(), hdr_line.size(), 0) <= 0) {
//                completed = true;
//                final_var.notify_all();
//                return false;
//            }

            fwrite(data, 1, size, f);

            if (::send(socket, data, size, 0) <= 0) {
                completed = true;
                final_var.notify_all();
                return false;
            }

//            if(::send(socket, "\r\n", 2, 0) <= 0) {
//                completed = true;
//                final_var.notify_all();
//                return false;
//            }

            return true;
        }, [&completed, &final_var, socket, f]() {
            fclose(f);
            std::cout << "DONE" << std::endl;
//            ::send(socket, "0\r\n\r\n", 5, 0);
            completed = true;
            final_var.notify_all();
        });

        std::unique_lock<std::mutex> l{final_lock};
        final_var.wait(l, [&completed]() { return completed; });
    }
}