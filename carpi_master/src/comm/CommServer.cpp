#include <common_utils/error.hpp>
#include "CommServer.hpp"

namespace carpi {
    LOGGER_IMPL(CommServer);

    CommServer::CommServer() {
        _server = std::make_shared<bluetooth::BluetoothServer>(0x01);
        _network_thread = std::thread{[=]() { network_runner(); }};
    }

    void CommServer::shutdown_acceptor(){
        _is_running = false;

        if(_network_thread.joinable()) {
            _network_thread.join();
        }
    }

    void CommServer::network_runner() {
        while(_is_running) {
            fd_set socket_set{};
            FD_ZERO(&socket_set);
            auto max_socket = _server->fd();

            FD_SET(_server->fd(), &socket_set);

            for(const auto& connection : _connections) {
                FD_SET(connection->fd(), &socket_set);
                max_socket = std::max(max_socket, connection->fd());
            }

            timeval timeout{
                .tv_sec = 1,
                .tv_usec = 0
            };

            const auto rc = select(max_socket + 1, &socket_set, nullptr, nullptr, &timeout);
            log->debug("Selected sockets. Readable sockets: {}", rc);

            if(rc < 0) {
                log->error("Error selecting from sockets: {} (errno={})", utils::error_to_string(errno), errno);
                return; // TODO: trigger error
            }

            if(rc > 0) {
                if(FD_ISSET(_server->fd(), &socket_set)) {
                    const auto connection = _server->accept_connection();
                    log->info("Accepted new bluetooth connection: {}", connection->to_string());
                    _connections.emplace_back(connection);
                }

                // TODO: handle active connections
            }
        }
    }
}
