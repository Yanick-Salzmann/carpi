#ifndef CARPI_BLUETOOTH_BROADCAST_HPP
#define CARPI_BLUETOOTH_BROADCAST_HPP

#include <string>
#include <list>
#include <common_utils/log.hpp>
#include <ipc_common/ipc_package.hpp>
#include <gps/gps_listener.hpp>
#include "bluetooth_utils/BluetoothConnection.hpp"

namespace carpi::gps {
    class BluetoothBroadcast {
        typedef std::shared_ptr<bluetooth::BluetoothConnection> BtConnPtr;

        LOGGER;

        bool _is_active_mode = false;
        BtConnPtr _client_connection{};
        std::list<BtConnPtr> _server_connections;
        std::mutex _connection_lock;

        void init_server_mode();
        void init_client_mode(std::string device_address);

        void send_packet(const ipc::IpcPackage& package);

    public:
        BluetoothBroadcast(const std::string& mode, std::string target_device);

        void on_measurement(const GpsMeasurement& measurement);
    };
}

#endif //CARPI_BLUETOOTH_BROADCAST_HPP
