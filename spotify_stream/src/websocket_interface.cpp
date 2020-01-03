#include "websocket_interface.hpp"
#include "api_gateway.hpp"
#include <nlohmann/json.hpp>
#include <chrono>
#include <common_utils/string.hpp>
#include "state_machine.hpp"

namespace carpi::spotify {
    LOGGER_IMPL(WebsocketInterface);

    using nlohmann::json;

    WebsocketInterface::WebsocketInterface(const std::string &access_token) {
        initialize_command_map();
        _client = std::make_shared<net::WebsocketClient>(fmt::format("wss://{}/?access_token={}", sApiGateway->gew_dealer(), access_token), std::bind(&WebsocketInterface::packet_received, this, std::placeholders::_1));
        _ping_thread = std::thread{[=]() { ping_loop(); }};
    }

    WebsocketInterface::~WebsocketInterface() {
        _is_running = false;
        if (_ping_thread.joinable()) {
            _ping_thread.join();
        }
    }

    void WebsocketInterface::packet_received(const std::string &payload) {
        json value{};
        try {
            value = json::parse(payload);
        } catch (std::exception &e) {
            log->warn("Unknown websocket packet received: {}", payload);
            return;
        }

        std::string type;
        std::map<std::string, std::string> headers;

        try {
            type = value["type"];
        } catch (std::exception &e) {
            log->warn("Error processing packet '{}', missing type attribute", payload);
            return;
        }

        const auto header_itr = value.find("headers");
        if (header_itr != value.end()) {
            for (const auto&[key, value] : (*header_itr).items()) {
                headers.emplace(utils::to_lower(key), value);
            }
        }

        log->info(">> {}", type);

        if (type == "pong") {
            return;
        }

        if (consume_connection_packet(headers)) {
            return;
        }

        if (consume_command(value)) {
            return;
        }
    }

    void WebsocketInterface::ping_loop() {
        typedef std::chrono::steady_clock sc;
        typedef std::chrono::steady_clock::time_point tp;

        auto last_ping = sc::now();
        _client->send_message(R"({"type":"ping"})");

        while (_is_running) {
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - last_ping).count();
            if (diff >= 20000) {
                last_ping = sc::now();
                _client->send_message(R"({"type":"ping"})");
                continue;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{50});
        }
    }

    bool WebsocketInterface::consume_connection_packet(const std::map<std::string, std::string> &headers) {
        if (_has_connection_id) {
            return false;
        }

        const auto conn_key_itr = headers.find("spotify-connection-id");
        if (conn_key_itr != headers.end()) {
            _has_connection_id = true;
            _connection_id = conn_key_itr->second;
            log->info("New Spotify connection: {}", _connection_id);
            _connection_variable.notify_all();
            return true;
        }

        return false;
    }

    void WebsocketInterface::wait_for_login() {
        std::unique_lock<std::mutex> l{_connection_lock};
        _connection_variable.wait(l);
    }

    bool WebsocketInterface::consume_command(const nlohmann::json &payload) {
        const auto uri_itr = payload.find("uri");
        const auto payloads_itr = payload.find("payloads");

        if (uri_itr == payload.end() || payloads_itr == payload.end() || (*payloads_itr).empty()) {
            return false;
        }

        const std::string uri = *uri_itr;
        if (utils::to_lower(uri) != "hm://track-playback/v1/command") {
            return false;
        }

        const auto payloads = (*payloads_itr)[0];
        const auto type_itr = payloads.find("type");
        if (type_itr == payloads.end()) {
            return true;
        }

        const auto type = utils::to_lower(*type_itr);
        const auto handler_itr = _command_map.find(type);
        if (handler_itr != _command_map.end()) {
            handler_itr->second(payloads);
        }

        return true;
    }

    void WebsocketInterface::handle_replace_state(nlohmann::json payload) {
        if(_state_machine) {
            _state_machine->handle_state_replace(payload);
        }
    }

    void WebsocketInterface::initialize_command_map() {
        _command_map = {
                {"replace_state", [=](nlohmann::json payload) { handle_replace_state(payload); }}
        };
    }
}