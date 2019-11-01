#include "data_server/http_connection.hpp"

#include <sys/socket.h>
#include <common_utils/string.hpp>
#include <unistd.h>

#include "data_server/http_request.hpp"

namespace carpi::data {
    LOGGER_IMPL(HttpConnection);

    HttpConnection::HttpConnection(int32_t socket) : _socket{socket} {
        _request_thread = std::thread{[=]() { request_loop(); }};
    }

    void HttpConnection::request_loop() {
        while(_is_running) {
            const auto method = utils::trim(read_until(' '));
            const auto path = utils::trim(read_until(' '));
            const auto version = utils::trim(read_until("\r\n"));

            log->info("Got request: Method: {}, Path: {}, Version: {}", method, path, version);
            HttpRequest req{method, path, version, _socket};
            break;
        }

        shutdown();
    }

    void HttpConnection::shutdown() {
        if(!_is_running) {
            return;
        }

        _is_running = false;
        ::shutdown(_socket, SHUT_RDWR);
        close(_socket);

        if(_request_thread.joinable()) {
            _request_thread.join();
        }
    }

    std::string HttpConnection::read_until(const std::string &delimiter) {
        std::string cur_part{};
        const auto is_single_char_delimiter = delimiter.length() == 1;

        while(true) {
            char chr{};
            if(recv(_socket, &chr, sizeof chr, 0) != sizeof chr) {
                break;
            }

            cur_part += chr;
            if(is_single_char_delimiter && delimiter[0] == chr) {
                return cur_part;
            } else if(cur_part.find(delimiter) != std::string::npos) {
                return cur_part;
            }
        }

        return {};
    }
}