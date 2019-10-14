#ifndef CARPI_BLUETOOTH_UTILS_BLUETOOTHSERVER_HPP
#define CARPI_BLUETOOTH_UTILS_BLUETOOTHSERVER_HPP

#include <sys/socket.h>
#include <memory>

#include "common_utils/log/Logger.hpp"

namespace carpi::bluetooth {
    class BluetoothConnection;

    class BluetoothServer {
        LOGGER;

        int _socket = -1;

    public:
        BluetoothServer(uint16_t psm, uint16_t cid);

        std::shared_ptr<BluetoothConnection> accept_connection();
    };
}

#endif //CARPI_BLUETOOTH_UTILS_BLUETOOTHSERVER_HPP
