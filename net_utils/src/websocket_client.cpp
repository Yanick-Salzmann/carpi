#include "net_utils/websocket_client.hpp"
#include <common_utils/string.hpp>
#include <net_utils/http_request.hpp>
#include <common_utils/conversion.hpp>
#include <common_utils/random.hpp>
#include "net_utils/ssl_socket.hpp"

#if !CURL_AT_LEAST_VERSION(7, 62, 0)
#error "this example requires curl 7.62.0 or later"
#endif

namespace carpi::net {
    LOGGER_IMPL(WebsocketClient);

    WebsocketClient::WebsocketClient(const std::string &wss_address) : _target_url{UrlParser::parse(wss_address)}, _raw_address{wss_address} {
        verify_wss_url();

        _socket = std::make_shared<SslSocket>();

        const auto port = static_cast<uint16_t>(_target_url.port > 0 ? _target_url.port : 443);
        _socket->connect(_target_url.host, port);

        log->info("Socket connected: {}", _socket->to_string());
        upgrade_connection();
    }

    void WebsocketClient::verify_wss_url() {
        const auto scheme = utils::to_lower(_target_url.scheme);
        if (scheme != "wss") {
            log->error("Scheme '{}' is not supported. Must be 'wss' for secure web socket.", scheme);
            throw std::runtime_error{"Unsupported URL scheme"};
        }

        if (_target_url.host.empty()) {
            log->error("No host set in web socket URL");
            throw std::runtime_error{"Invalid websocket URL"};
        }
    }

    void WebsocketClient::upgrade_connection() {
        HttpRequest request{"GET", _raw_address};
        request.add_header("Connection", "Upgrade")
                .add_header("Pragma", "no-cache")
                .add_header("User-Agent", "carpi")
                .add_header("Upgrade", "websocket")
                .add_header("Origin", "http://localhost:8888")
                .add_header("Sec-WebSocket-Version", "13")
                .add_header("Accept-Encoding", "gzip, deflate, br")
                .add_header("Accept-Language", "en-US")
                .add_header("Accept", "application/json")
                .add_header("Sec-WebSocket-Extensions", "permessage-deflate; client_max_window_bits")
                .add_header("Sec-WebSocket-Key", utils::base64_encode(utils::random_bytes(16)));

        const auto data = request.send_data();
        _socket->write(data.data(), data.size());

        verify_upgrade_response();
    }

    void WebsocketClient::verify_upgrade_response() {
        const auto status_line = read_one_http_response_line();
        log->info(status_line);

        std::multimap<std::string, std::string> response_headers{};
        std::string header_line = read_one_http_response_line();
        while(!header_line.empty()) {
            const auto idx_colon = header_line.find(':');
            if(idx_colon == std::string::npos) {
                response_headers.emplace(utils::to_lower(header_line), "");
            } else {
                response_headers.emplace(utils::to_lower(utils::trim(header_line.substr(0, idx_colon))), utils::trim(header_line.substr(idx_colon + 1)));
            }

            header_line = read_one_http_response_line();
        }

        const auto content_length = response_headers.find("content-length");
        if(content_length == response_headers.end()) {
            log->warn("No content-length header in response found, cannot read body, assuming there is none");
            return;
        }

        const auto length = std::stoull(content_length->second);
        std::vector<uint8_t> body_buffer(length);
        _socket->read_all(body_buffer.data(), length);
        body_buffer.emplace_back('\0');
        body_buffer.emplace_back('\0');

        std::string body = (const char*) body_buffer.data();
        log->info(body);
    }

    std::string WebsocketClient::read_one_http_response_line() {
        std::vector<uint8_t> ret{};
        auto has_cr = false;
        while(true) {
            const auto chr = _socket->read_one();
            if(chr == '\r' && !has_cr) {
                has_cr = true;
                continue;
            }

            if(chr == '\n') {
                if(has_cr) {
                    break;
                }
            }

            if(has_cr) {
                ret.emplace_back('\r');
                has_cr = false;
            }

            ret.emplace_back(chr);
        }

        ret.emplace_back('\0');
        return (const char*) ret.data();
    }
}