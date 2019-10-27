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
        const auto url = request->GetURL();
        CefURLParts url_parts{};
        if (!CefParseURL(url, url_parts)) {
            log->warn("Error parsing URL in CEF request: {}", url.ToString());
            handle_request = true;
            _has_file_error = true;
            return true;
        }

        auto url_path = CefString{&url_parts.path}.ToString();
        if (url_path.empty()) {
            log->warn("Invalid empty URL path in CEF request: {}", url.ToString());
            _has_file_error = true;
            handle_request = true;
            return true;
        }

        if (url_path[0] == '/' && url_path.size() >= 2 && url_path[1] == '/') {
            url_path = url_path.substr(2);
        }

        std::filesystem::path file_path;
        try {
            file_path = std::filesystem::path{url_path};
        } catch (std::exception &e) {
            _has_file_error = true;
            _file_error = e.what();
            handle_request = true;
            return true;
        }

        auto target_path = std::filesystem::path{"./../../carpi_master"};
        try {
            target_path /= file_path;
        } catch (std::exception &e) {
            _has_file_error = true;
            _file_error = e.what();
            handle_request = true;
            return true;
        }

        if (!exists(target_path)) {
            _has_file_error = true;
            handle_request = true;
            return true;
        }

        _extension = file_path.extension().string();

        log->info("Serving {} from {}", url.ToString(), canonical(absolute(target_path)).string());

        _fd = fopen(target_path.string().c_str(), "rb");
        if (_fd != nullptr) {
            _is_found = true;
            fseek(_fd, 0, SEEK_END);
            _file_size = ftell(_fd);
            fseek(_fd, 0, SEEK_SET);
        } else {
            _is_found = false;
        }

        handle_request = true;
        return true;
    }

    void LocalSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) {
        if (!_is_found || _has_file_error) {
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
        }
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
        log->info("Position: {}, File Size: {}, To Read: {}, Available: {}", _position, _file_size, bytes_to_read, available);
        if (available <= 0) {
            bytes_read = 0;
            return false;
        }

        const auto to_read = std::min<int64_t>(bytes_to_read, available);
        log->info("Reading: {}", to_read);
        fread(data_out, 1, to_read, _fd);
        _position += to_read;
        bytes_read = to_read;
        return true;
    }
}
