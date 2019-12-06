#ifndef CARPI_BLUETOOTH_BROADCAST_HPP
#define CARPI_BLUETOOTH_BROADCAST_HPP

#include <string>
#include <list>
#include <common_utils/log.hpp>
#include <ipc_common/ipc_package.hpp>
#include <gps/gps_listener.hpp>
#include "bluetooth_utils/bluetooth_connection.hpp"
#include "bluetooth_utils/bluetooth_server.hpp"

namespace carpi::gps {
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

        void send_packet(const ipc::IpcPackage& package);

        void passive_mode();

    public:
        BluetoothBroadcast(const std::string& mode, std::string target_device, uint8_t channel);
        ~BluetoothBroadcast();

        BluetoothBroadcast(BluetoothBroadcast&) = delete;
        BluetoothBroadcast(BluetoothBroadcast&&) = delete;

        void operator = (BluetoothBroadcast&) = delete;
        void operator = (BluetoothBroadcast&&) = delete;

        void on_measurement(const GpsMeasurement& measurement);
    };
}

#endif //CARPI_BLUETOOTH_BROADCAST_HPP
