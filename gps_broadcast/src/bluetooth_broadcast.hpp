#ifndef CARPI_BLUETOOTH_BROADCAST_HPP
#define CARPI_BLUETOOTH_BROADCAST_HPP

#include <string>
#include <common_utils/log.hpp>

namespace carpi::gps {
    class BluetoothBroadcast {
        LOGGER;

        void init_server_mode();
        void init_client_mode();

    public:
        BluetoothBroadcast(const std::string& mode, std::string target_device);
    };
}

#endif //CARPI_BLUETOOTH_BROADCAST_HPP
