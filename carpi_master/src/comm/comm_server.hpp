#ifndef CARPI_CARPI_MASTER_COMMSERVER_HPP
#define CARPI_CARPI_MASTER_COMMSERVER_HPP

#include <vector>
#include <memory>
#include <thread>

#include <bluetooth_utils/BluetoothServer.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>

#include <common_utils/log.hpp>

#include <sys/socket.h>

namespace carpi {
    class CommServer {
        LOGGER;

        typedef std::shared_ptr<bluetooth::BluetoothServer> BluetoothServerPtr;
        typedef std::shared_ptr<bluetooth::BluetoothConnection> BluetoothConnectionPtr;

        BluetoothServerPtr _server;
        std::vector<BluetoothConnectionPtr> _connections{};

        std::thread _network_thread;

        bool _is_running = true;

        void network_runner();

    public:
        CommServer();

        void shutdown_acceptor();
    };
}

#endif //CARPI_CARPI_MASTER_COMMSERVER_HPP
