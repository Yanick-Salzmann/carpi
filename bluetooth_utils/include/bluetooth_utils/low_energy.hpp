#ifndef CARPI_BLUETOOTH_UTILS_LOW_ENERGY_HPP
#define CARPI_BLUETOOTH_UTILS_LOW_ENERGY_HPP

#include <cstdint>

namespace carpi::bluetooth {
    class GattServer {
        int32_t _socket;

    public:
        GattServer();
    };
}

#endif //CARPI_BLUETOOTH_UTILS_LOW_ENERGY_HPP
