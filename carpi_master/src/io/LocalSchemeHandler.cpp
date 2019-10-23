#include "LocalSchemeHandler.hpp"

#include <include/cef_parser.h>
#include <string>
#include <filesystem>
#include <fstream>

namespace carpi::io {
    LOGGER_IMPL(LocalSchemeHandler);

    bool LocalSchemeHandler::Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) {
        const auto url = request->GetURL();
        CefURLParts url_parts{};
        if(!CefParseURL(url, url_parts)) {
            log->warn("Error parsing URL in CEF request: {}", url);
            handle_request = true;
            return false;
        }

        const auto file_path = std::filesystem::path{CefString{&url_parts.path}};

        auto target_path = std::filesystem::path{"../../carpi_master/ui"};
        target_path /= file_path;

        _extension = file_path.extension().string();

        log->debug("Serving request from {}", canonical(absolute(target_path)).string());

        std::ifstream is{target_path.string(), std::ios::binary};
        if(is) {
            _is_found = true;
            is.seekg(0, std::ios::end);
            const auto size = is.tellg();
            is.seekg(0, std::ios::beg);
            _file_data.resize(size);
            is.read((char*) _file_data.data(), size);
        } else {
            _is_found = false;
        }

        handle_request = true;
        return true;
    }

    void LocalSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) {
        if(!_is_found) {
            response->SetStatus(404);
            response->SetStatusText("File Not Found");
            response_length = 0;
        } else {
            const auto mime_type = CefGetMimeType(!_extension.empty() ? _extension.substr(1) : _extension);
            log->debug("Mime Type: {}, Extension: {}", mime_type, _extension);
            response_length = static_cast<int64_t>(_file_data.size());
            response->SetStatus(200);
            response->SetStatusText("OK");
            response->SetMimeType(mime_type.empty() ? "text/plain" : mime_type);
        }
    }

    bool LocalSchemeHandler::Skip(int64 bytes_to_skip, int64 &bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) {
        const auto available = (_position < _file_data.size()) ? _file_data.size() - _position : 0;
        const auto to_move = std::min<int64_t>(bytes_to_skip, available);
        _position += to_move;
        bytes_skipped = to_move;
        return true;
    }

    bool LocalSchemeHandler::Read(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefResourceReadCallback> callback) {
        const auto available = (_position < _file_data.size()) ? _file_data.size() - _position : 0;
        if(available <= 0) {
            bytes_read = 0;
            return false;
        }

        const auto to_read = std::min<int64_t>(bytes_to_read, available);
        memcpy(data_out, _file_data.data() + _position, to_read);
        _position += to_read;
        bytes_read = to_read;
        return true;
    }
}
