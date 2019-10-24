#ifndef CARPI_CARPI_IPCCLIENTMANAGER_HPP
#define CARPI_CARPI_IPCCLIENTMANAGER_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>

#include <vector>
#include <sys/socket.h>
#include <sys/un.h>

namespace carpi::comm {
    class IpcClientManager : public utils::Singleton<IpcClientManager> {
        LOGGER;

        std::vector<int> _pending_connections;

    public:
        void accept_connection(int connection, sockaddr_un addr);
    };
}

#define sIpcClientManager (carpi::comm::IpcClientManager::instance())

#endif //CARPI_CARPI_IPCCLIENTMANAGER_HPP
