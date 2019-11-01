#include "LocalSchemeHandler.hpp"

#include <include/cef_parser.h>
#include <string>
#include <filesystem>
#include <fstream>

namespace carpi::io {
    LOGGER_IMPL(LocalSchemeHandler);

    LocalSchemeHandler::~LocalSchemeHandler() {
        if (_fd != nullptr) {
            fclose(_fd);
        }
    }

    bool LocalSchemeHandler::Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) {
        CefRequest::HeaderMap headers{};
        request->GetHeaderMap(headers);

        for(const auto& header : headers) {
            log->info("{}: {}", header.first.ToString(), header.second.ToString());
        }

        handle_request = true;
        const auto url = request->GetURL();
        CefURLParts url_parts{};
        if (!CefParseURL(url, url_parts)) {
            log->warn("Error parsing URL in CEF request: {}", url.ToString());
            _has_file_error = true;
            return true;
        }

        auto url_path = CefString{&url_parts.path}.ToString();
        if (url_path.empty()) {
            log->warn("Invalid empty URL path in CEF request: {}", url.ToString());
            _has_file_error = true;
            return true;
        }

        log->info("Attempting to load file: {}", url_path);

        if (url_path[0] == '/') {
            url_path = url_path.substr(1);
        }

        std::filesystem::path file_path;
        try {
            file_path = std::filesystem::path{url_path};
        } catch (std::exception &e) {
            _has_file_error = true;
            _file_error = e.what();
            return true;
        }

        auto target_path = std::filesystem::path{"./../../carpi_master/ui"};
        try {
            target_path /= file_path;
        } catch (std::exception &e) {
            _has_file_error = true;
            _file_error = e.what();
            return true;
        }

        if (!exists(target_path)) {
            log->warn("File does not exist: {}", target_path.string());
            _has_file_error = true;
            return true;
        }

        _extension = file_path.extension().string();

        log->info("Serving {} from {}", url.ToString(), canonical(absolute(target_path)).string());

        _fd = fopen(target_path.string().c_str(), "rb");
        if (_fd != nullptr) {
            _file_name = target_path.string();
            _is_found = true;
            fseek(_fd, 0, SEEK_END);
            _file_size = ftell(_fd);
            fseek(_fd, 0, SEEK_SET);
        } else {
            _is_found = false;
        }

        return true;
    }

    void LocalSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) {
        CefResponse::HeaderMap headers;
        response->GetHeaderMap (headers);
        headers.emplace ("Access-Control-Allow-Origin", "*");

        if (!_is_found || _has_file_error) {
            log->warn("Did not find some file: {}", _file_name);
            response->SetStatus(404);
            response->SetStatusText(_has_file_error ? _file_error : "File Not Found");
            response_length = 0;
        } else {
            const auto mime_type = CefGetMimeType(!_extension.empty() ? _extension.substr(1) : _extension);
            log->info("Mime Type: {}, Extension: {}, Size: {}", mime_type.ToString(), _extension, _file_size);

            response_length = static_cast<int64_t>(_file_size);
            redirectUrl = "";

            response->SetStatus(200);
            response->SetStatusText("OK");
            response->SetMimeType(mime_type.empty() ? "text/plain" : mime_type);

            if(mime_type.ToString().find("mp4") != std::string::npos) {
                headers.emplace("Content-Range", fmt::format("bytes 0-{}/{}", _file_size, _file_size));
            }

            headers.emplace("Content-Length", std::to_string(_file_size));
        }

        response->SetHeaderMap (headers);
    }

    bool LocalSchemeHandler::Skip(int64 bytes_to_skip, int64 &bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) {
        const auto available = (_position < _file_size) ? (_file_size - _position) : 0;
        const auto to_move = std::min<int64_t>(bytes_to_skip, available);
        _position += to_move;
        bytes_skipped = to_move;
        fseek(_fd, to_move, SEEK_CUR);
        return true;
    }

    bool LocalSchemeHandler::Read(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefResourceReadCallback> callback) {
        const auto available = (_position < _file_size) ? (_file_size - _position) : 0;
        if (available <= 0) {
            bytes_read = 0;
            return false;
        }

        const auto to_read = std::min<int64_t>(bytes_to_read, available);
        fread(data_out, 1, to_read, _fd);
        _position += to_read;
        bytes_read = to_read;
        return true;
    }

    bool LocalSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
        auto handle_request = false;
        return Open(request, handle_request, callback);
    }

    bool LocalSchemeHandler::ReadResponse(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefCallback> callback) {
        return CefResourceHandler::ReadResponse(data_out, bytes_to_read, bytes_read, callback);
    }
}
