#ifndef CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP
#define CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP

#include <sys/socket.h>

namespace carpi::bluetooth {
    class BluetoothConnection {
        int _socket;

    public:
        explicit BluetoothConnection(int socket);
    };
}

#endif //CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP
