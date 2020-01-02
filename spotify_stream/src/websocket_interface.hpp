#ifndef CARPI_WEBSOCKET_INTERFACE_HPP
#define CARPI_WEBSOCKET_INTERFACE_HPP

#include <memory>
#include <net_utils/websocket_client.hpp>
#include "oauth/refresh_flow.hpp"
#include <common_utils/log.hpp>
#include <thread>
#include <nlohmann/json.hpp>
#include <functional>

namespace carpi::spotify {
    class WebsocketInterface {
        LOGGER;

        std::shared_ptr<net::WebsocketClient> _client;
        bool _is_running = true;
        bool _has_connection_id = false;

        std::map<std::string, std::function<void(nlohmann::json)>> _command_map;

        std::mutex _connection_lock;
        std::condition_variable _connection_variable{};

        std::string _connection_id{};

        std::thread _ping_thread;

        void ping_loop();

        void packet_received(const std::string& payload);

        bool consume_connection_packet(const std::map<std::string, std::string>& headers);
        bool consume_command(const nlohmann::json& payload);

        void handle_replace_state(nlohmann::json payload);

        void initialize_command_map();

    public:
        explicit WebsocketInterface(const std::string& access_token);
        ~WebsocketInterface();

        WebsocketInterface(const WebsocketInterface&) = delete;
        WebsocketInterface(WebsocketInterface&&) = delete;

        void operator = (const WebsocketInterface&) = delete;
        void operator = (WebsocketInterface&&) = delete;

        void wait_for_login();

        [[nodiscard]] std::string connection_id() const {
            return _connection_id;
        }
    };
}

#endif //CARPI_WEBSOCKET_INTERFACE_HPP
