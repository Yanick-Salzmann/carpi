#include "data_server/http_connection.hpp"

#include <sys/socket.h>
#include <common_utils/string.hpp>
#include <unistd.h>

#include "data_server/http_request.hpp"

namespace carpi::data {
    LOGGER_IMPL(HttpConnection);

    HttpConnection::HttpConnection(int32_t socket) : _socket{socket} {
        linger l {
            .l_onoff = 1,
            .l_linger = 0
        };
        setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof l);
        _request_thread = std::thread{[=]() { request_loop(); }};
    }

    void HttpConnection::request_loop() {
        while (_is_running) {
            const auto method = utils::trim(read_until(' '));
            const auto path = utils::trim(read_until(' '));
            const auto version = utils::trim(read_until("\r\n"));

            log->info("Got request: Method: {}, Path: {}, Version: {}", method, path, version);
            auto header_line = utils::trim(read_until("\r\n"));
            std::multimap<std::string, std::string> header_map{};

            while(!header_line.empty()) {
                auto hdr_end = header_line.find(':');
                if(hdr_end == std::string::npos) {
                    header_map.emplace(header_line, std::string{});
                } else {
                    const auto key = utils::trim(header_line.substr(0, hdr_end));
                    const auto value = utils::trim_left(header_line.substr(hdr_end + 1));
                    header_map.emplace(utils::to_lower(key), value);
                }

                header_line = utils::trim(read_until("\r\n"));
            }

            HttpRequest req{method, path, version, header_map, _socket};
            break;
        }

        shutdown(false);
    }

    void HttpConnection::shutdown(bool join) {
        if (!_is_running) {
            return;
        }

        _is_running = false;
        ::shutdown(_socket, SHUT_RDWR);
        close(_socket);

        if (join) {
            if (_request_thread.joinable()) {
                _request_thread.join();
            }
        }
    }

    std::string HttpConnection::read_until(const std::string &delimiter) {
        std::string cur_part{};
        const auto is_single_char_delimiter = delimiter.length() == 1;

        while (true) {
            char chr{};
            if (recv(_socket, &chr, sizeof chr, 0) != sizeof chr) {
                break;
            }

            cur_part += chr;
            if (is_single_char_delimiter && delimiter[0] == chr) {
                return cur_part;
            } else if (cur_part.find(delimiter) != std::string::npos) {
                return cur_part;
            }
        }

        return {};
    }
}