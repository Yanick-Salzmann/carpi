#include "data_server/http_response.hpp"
#include <sstream>

#include <sys/socket.h>
#include <common_utils/string.hpp>
#include <iostream>

namespace carpi::data {
    LOGGER_IMPL(HttpResponse);

    HttpResponse::HttpResponse(HttpStatusCode status, std::string status_text) : _status_code{(int32_t) status}, _status_text{std::move(status_text)} {

    }

    HttpResponse::HttpResponse(int32_t status, std::string status_text) : _status_code{status}, _status_text{std::move(status_text)} {

    }

    HttpResponse::~HttpResponse() {
        if(_response_file != nullptr) {
            fclose(_response_file);
            _response_file = nullptr;
        }
    }

    HttpResponse& HttpResponse::add_header(const std::string &name, const std::string &value) {
        _headers.emplace(utils::to_lower(name), value);
        return *this;
    }

    void HttpResponse::write_to_socket(int socket) {
        std::stringstream hdr_line;
        hdr_line << "HTTP/1.1 " << _status_code << " " << _status_text << "\r\n";

        if(_response_file != nullptr && _headers.find("content-length") == _headers.end()) {
            fseek(_response_file, 0, SEEK_END);
            const auto response_size = ftell(_response_file);
            fseek(_response_file, 0, SEEK_SET);
            add_header("Content-Length", std::to_string(response_size));
        }

        for(const auto& hdr : _headers) {
            hdr_line << hdr.first << ": " << hdr.second << "\r\n";
        }
        hdr_line << "\r\n";

        const auto hdr_string = hdr_line.str();
        ::send(socket, hdr_string.c_str(), hdr_string.size(), 0);

        if(_response_file != nullptr) {
            std::size_t num_sent = 0;
            char buffer[16384]{};
            auto num_read = 0;
            do {
                num_read = fread(buffer, 1, sizeof buffer, _response_file);
                if(num_read > 0) {
                    ::send(socket, buffer, num_read, 0);
                    num_sent += num_read;
                }
            } while(num_read > 0);
            fclose(_response_file);
            log->info("Sent {} bytes", num_sent);
        }

        _response_file = nullptr;
    }

    HttpResponse& HttpResponse::respond_with_file(const std::string &path) {
        _response_file = fopen(path.c_str(), "rb");
        return *this;
    }
}