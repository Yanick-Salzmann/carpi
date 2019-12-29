#ifndef CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP
#define CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP

#include <string>
#include <memory>
#include <common_utils/log.hpp>
#include "url_parser.hpp"

namespace carpi::net {
    class SslSocket;

    class WebsocketClient {
        LOGGER;

        Url _target_url;
        std::string _raw_address;
        std::shared_ptr<SslSocket> _socket;

        std::string read_one_http_response_line();

        void verify_wss_url();

        void upgrade_connection();

        void verify_upgrade_response();

    public:
        /**
         * Create and connect as a websocket client to the given address.
         * Only secure websockets (wss://...) are supported.
         *
         * @param wss_address full URL for the websocket (only wss:// supported)
         */
        explicit WebsocketClient(const std::string& wss_address);
    };
}

#endif //CARPI_NET_UTILS_WEBSOCKET_CLIENT_HPP
