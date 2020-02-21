#ifndef CARPI_CARPI_IPCCLIENTMANAGER_HPP
#define CARPI_CARPI_IPCCLIENTMANAGER_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>

#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/un.h>
#include <ipc_common/ipc_type.hpp>
#include <bluetooth_utils/bluetooth_connection.hpp>

namespace carpi::comm {
    class IpcConnection;
    typedef std::shared_ptr<IpcConnection> IpcConnectionPtr;

    class IpcClientManager : public utils::singleton<IpcClientManager> {
        LOGGER;

        std::map<ipc::IpcType, IpcConnectionPtr> _connections;

        bool read_type_from_socket(int socket, ipc::IpcType& out_type);

    public:
        void accept_connection(int connection, sockaddr_un addr);
        void accept_bluetooth(std::shared_ptr<bluetooth::BluetoothConnection> connection);
    };
}

#define sIpcClientManager (carpi::comm::IpcClientManager::instance())

#endif //CARPI_CARPI_IPCCLIENTMANAGER_HPP
