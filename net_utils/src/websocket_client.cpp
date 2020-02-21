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

    WebsocketClient::WebsocketClient(const std::string &wss_address, std::function<void(const std::string &)> callback) :
            _target_url{url_parser::parse(wss_address)},
            _raw_address{wss_address},
            _packet_callback{std::move(callback)} {
        verify_wss_url();

        _socket = std::make_shared<ssl_socket>();

        const auto port = static_cast<uint16_t>(_target_url.port > 0 ? _target_url.port : 443);
        _socket->connect(_target_url.host, port);

        log->info("Socket connected: {}", _socket->to_string());
        upgrade_connection();

        _read_thread = std::thread{[this]() { read_loop(); }};
    }

    WebsocketClient::~WebsocketClient() {
        _socket->shutdown();

        _is_running = false;

        if (_read_thread.joinable()) {
            _read_thread.join();
        }
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
        http_request request{"GET", _raw_address};
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
        while (!header_line.empty()) {
            const auto idx_colon = header_line.find(':');
            if (idx_colon == std::string::npos) {
                response_headers.emplace(utils::to_lower(header_line), "");
            } else {
                response_headers.emplace(utils::to_lower(utils::trim(header_line.substr(0, idx_colon))), utils::trim(header_line.substr(idx_colon + 1)));
            }

            header_line = read_one_http_response_line();
        }

        const auto is_upgrade_ok = is_successful_upgrade(status_line, response_headers);

        const auto content_length = response_headers.find("content-length");
        if (content_length == response_headers.end()) {
            if (is_upgrade_ok) {
                return;
            }

            log->warn("No content-length header in response found, cannot read body, assuming there is none");
            throw std::runtime_error{"Websocket Upgrade failed"};
        }

        const auto length = std::stoull(content_length->second);
        std::vector<uint8_t> body_buffer(length);
        _socket->read_all(body_buffer.data(), length);
        body_buffer.emplace_back('\0');
        body_buffer.emplace_back('\0');

        std::string body = (const char *) body_buffer.data();
        log->info(body);

        if (!is_upgrade_ok) {
            throw std::runtime_error{"Websocket Upgrade failed"};
        }
    }

    std::string WebsocketClient::read_one_http_response_line() {
        std::vector<uint8_t> ret{};
        auto has_cr = false;
        while (true) {
            const auto chr = _socket->read_one();
            if (chr == '\r' && !has_cr) {
                has_cr = true;
                continue;
            }

            if (chr == '\n') {
                if (has_cr) {
                    break;
                }
            }

            if (has_cr) {
                ret.emplace_back('\r');
                has_cr = false;
            }

            ret.emplace_back(chr);
        }

        ret.emplace_back('\0');
        return (const char *) ret.data();
    }

    bool WebsocketClient::is_successful_upgrade(const std::string &status_line, const std::multimap<std::string, std::string> &headers) {
        const auto idx_space_first = status_line.find(' ');
        if (idx_space_first == std::string::npos) {
            return false;
        }

        const auto idx_space = status_line.find(' ', idx_space_first + 1);
        if (idx_space == std::string::npos) {
            return false;
        }

        try {
            if (std::stoi(status_line.substr(idx_space_first + 1, (idx_space - idx_space_first) - 1)) != 101) {
                return false;
            }
        } catch (std::invalid_argument &) {
            return false;
        }

        const auto upgrade_header = headers.find("upgrade");
        if (upgrade_header == headers.end()) {
            return false;
        }

        return utils::to_lower(utils::trim(upgrade_header->second)) == "websocket";
    }

    void WebsocketClient::read_loop() {
        std::vector<uint8_t> full_payload{};
        while (_is_running) {
            try {
                const auto b0 = _socket->read_one();
                const auto b1 = _socket->read_one();

                const auto fin = (b0 & 0x80u) != 0;
                const auto opcode = b0 & 0x0Fu;

                const auto masked = (b1 & 0x80u) != 0;
                uint64_t length = b1 & 0x7Fu;

                if (length == 126) {
                    length = (((uint16_t) _socket->read_one()) << 8u) | _socket->read_one();
                } else if (length == 127) {
                    length = 0;
                    for (auto i = 0; i < 8; ++i) {
                        length |= ((uint64_t) _socket->read_one()) << (56 - i * 8);
                    }
                }

                std::vector<uint8_t> payload(length);
                _socket->read_all(payload.data(), payload.size());

                full_payload.insert(full_payload.end(), payload.begin(), payload.end());
                if (fin) {
                    full_payload.emplace_back('\0');
                    full_payload.emplace_back('\0');
                    std::string data = (const char *) full_payload.data();
                    full_payload.clear();
                    _packet_callback(data);
                } else {
                    if (opcode != 0) {
                        log->warn("Expected continuation opcode (0) but was {}", opcode);
                    }
                }

            } catch (std::runtime_error &e) {
                if (!_is_running) {
                    return;
                }

                throw e;
            }
        }
    }

    void WebsocketClient::send_message(const std::string &message) {
        std::vector<uint8_t> buffer{};
        buffer.reserve(message.size() + 12);

        buffer.emplace_back(0x81);

        uint8_t b1 = 0x80; /* MASK */
        const auto size = message.size();

        if (size > 125) {
            if (size > std::numeric_limits<uint16_t>::max()) {
                b1 |= 127;
                buffer.emplace_back(b1);
                for (auto i = 0; i < 6; ++i) {
                    buffer.emplace_back((size >> (i * 8u)) & 0xFF);
                }
            } else {
                b1 |= 126;
                buffer.emplace_back(b1);
                for (auto i = 0; i < 2; ++i) {
                    buffer.emplace_back((size >> (i * 8u)) & 0xFF);
                }
            }
        } else {
            b1 |= (uint8_t) size;
            buffer.emplace_back(b1);
        }

        for (auto i = 0; i < 4; ++i) {
            buffer.emplace_back(_client_mask >> (i * 8u));
        }

        auto mask_index = 0;
        for (const auto &chr : message) {
            buffer.emplace_back(((uint8_t) chr) ^ ((_client_mask >> (mask_index++ * 8u)) & 0xFF));
            mask_index %= 4;
        }

        _socket->write(buffer.data(), buffer.size());
    }
}