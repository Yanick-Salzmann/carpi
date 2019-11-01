#include "data_server/http_server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <common_utils/error.hpp>
#include <arpa/inet.h>

#include "data_server/http_connection.hpp"

namespace carpi::data {
    LOGGER_IMPL(HttpServer);

    HttpServer::HttpServer(uint16_t port) {
        initialize_socket(port);
        _acceptor_thread = std::thread{[this]() { acceptor_loop(); }};
    }

    HttpServer::~HttpServer() {
        shutdown();
    }

    void HttpServer::initialize_socket(uint16_t port) {
        _server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (_server_socket < 0) {
            log->error("Error creating server socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating HTTP server"};
        }

        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        auto rc = bind(_server_socket, (const sockaddr *) &sin, sizeof sin);
        if (rc < 0) {
            log->error("Error binding socket to port {}: {} (errno={})", port, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error binding HTTP server to port"};
        }

        rc = listen(_server_socket, 10);
        if (rc < 0) {
            log->error("Error starting listening on socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error listening on HTTP server port"};
        }
    }

    void HttpServer::shutdown() {
        if (!_is_running) {
            return;
        }

        _is_running = false;

        ::shutdown(_server_socket, SHUT_RDWR);
        close(_server_socket);

        if (_acceptor_thread.joinable()) {
            _acceptor_thread.join();
        }

        for(auto& connection : _active_connections) {
            connection->shutdown();
        }

        _active_connections.clear();
    }

    void HttpServer::acceptor_loop() {
        while(_is_running) {
            sockaddr_in client_addr{};
            socklen_t addr_len = sizeof client_addr;
            const auto rc = accept(_server_socket, (sockaddr*) &client_addr, &addr_len);
            if(rc < 0) {
                if(errno == EBADF) {
                    break;
                }

                log->error("Unexpected error accepting http client: {} (errno={})", utils::error_to_string(errno), errno);
                throw std::runtime_error{"Error accepting socket"};
            }

            std::string remote_addr = inet_ntoa(client_addr.sin_addr);
            log->info("Accepted HTTP client from {}:{}", remote_addr, ntohs(client_addr.sin_port));
            auto connection = std::make_shared<HttpConnection>(rc);
            _active_connections.emplace_back(connection);
        }
    }
}