#ifndef CARPI_IPC_UTILS_BLUETOOTH_BROADCAST_HPP
#define CARPI_IPC_UTILS_BLUETOOTH_BROADCAST_HPP

#include <common_utils/log.hpp>
#include <bluetooth_utils/bluetooth_connection.hpp>
#include <bluetooth_utils/bluetooth_server.hpp>
#include "ipc_package.hpp"
#include <list>
#include <thread>
#include <memory>

namespace carpi::ipc {
    class BluetoothBroadcast {
        typedef std::shared_ptr<bluetooth::BluetoothConnection> BtConnPtr;

        LOGGER;

        bool _is_active_mode = false;
        BtConnPtr _client_connection{};

        std::list<BtConnPtr> _server_connections;
        std::mutex _connection_lock;
        std::thread _listener_thread;
        bool _is_running = true;

        std::shared_ptr<bluetooth::BluetoothServer> _server;

        void init_server_mode(uint8_t channel);
        void init_client_mode(std::string device_address, uint8_t channel);

        void passive_mode();

    public:
        BluetoothBroadcast(const std::string& mode, std::string target_device, uint8_t channel);
        ~BluetoothBroadcast();

        BluetoothBroadcast(BluetoothBroadcast&) = delete;
        BluetoothBroadcast(BluetoothBroadcast&&) = delete;

        void operator = (BluetoothBroadcast&) = delete;
        void operator = (BluetoothBroadcast&&) = delete;

        void send_packet(const IpcPackage& package);
    };
}

#endif //CARPI_IPC_UTILS_BLUETOOTH_BROADCAST_HPP
