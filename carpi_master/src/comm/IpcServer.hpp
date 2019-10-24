#ifndef CARPI_CARPI_IPCSERVER_HPP
#define CARPI_CARPI_IPCSERVER_HPP

#include <sys/socket.h>
#include <common_utils/log.hpp>

namespace carpi::comm {
    class IpcServer {
        LOGGER;

        int _server = 0;

        bool _is_running = true;
        std::thread _acceptor_thread;

        void handle_client_acceptor();

    public:
        IpcServer();
    };
}

#endif //CARPI_CARPI_IPCSERVER_HPP
