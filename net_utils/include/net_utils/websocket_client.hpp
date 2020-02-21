#ifndef CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP
#define CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP

#include <string>
#include <memory>
#include <common_utils/log.hpp>
#include "url_parser.hpp"
#include <map>
#include <common_utils/random.hpp>
#include <functional>

namespace carpi::net {
    class ssl_socket;

    class WebsocketClient {
        LOGGER;

        uint32_t _client_mask = utils::random_uint32();

        std::function<void (const std::string&)> _packet_callback;

        parsed_url _target_url;
        std::string _raw_address;
        std::shared_ptr<ssl_socket> _socket;

        std::thread _read_thread;

        bool _is_running = true;

        void read_loop();

        std::string read_one_http_response_line();

        void verify_wss_url();

        void upgrade_connection();

        void verify_upgrade_response();

        bool is_successful_upgrade(const std::string& status_line, const std::multimap<std::string, std::string>& headers);

    public:
        /**
         * Create and connect as a websocket client to the given address.
         * Only secure websockets (wss://...) are supported.
         *
         * @param wss_address full URL for the websocket (only wss:// supported)
         */
        explicit WebsocketClient(const std::string& wss_address, std::function<void (const std::string&)> callback);

        WebsocketClient(WebsocketClient&) = delete;
        WebsocketClient(WebsocketClient&&) = delete;

        ~WebsocketClient();

        void send_message(const std::string& message);
    };
}

#endif //CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP
