#include <common_utils/error.hpp>
#include "IpcServer.hpp"

#include <sys/un.h>

namespace carpi::comm {
    LOGGER_IMPL(IpcServer);

    IpcServer::IpcServer() {
        _server = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_server < 0) {
            log->error("Error creating unix IPC communication socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating UNIX socket"};
        }

        sockaddr_un sun{};
        sun.sun_family = AF_UNIX;
        strcpy(sun.sun_path, "carpi.comm.socket");

        auto rc = bind(_server, (const sockaddr *) &sun, sizeof sun);
        if (rc < 0) {
            log->error("Error binding unix socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating UNIX socket"};
        }

        rc = listen(_server, 10);
        if (rc < 0) {
            log->error("Error creating listener for unix socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating UNIX socket"};
        }

        _is_running = true;
        _acceptor_thread = std::thread{[this]() { handle_client_acceptor(); }};
    }

    void IpcServer::handle_client_acceptor() {
        while(_is_running) {
            sockaddr_un sun{};
            socklen_t len = sizeof sun;
            const auto client = accept(_server, (sockaddr*) &sun, &len);
            log->info("Accept unix comm socket from {}", sun.sun_path);

        }
    }
}