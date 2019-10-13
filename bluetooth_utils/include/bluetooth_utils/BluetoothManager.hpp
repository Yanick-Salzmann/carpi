#ifndef CARPI_BLUETOOTH_BLUETOOTH_MANAGER_H
#define CARPI_BLUETOOTH_BLUETOOTH_MANAGER_H

#include <string>
#include <stdexcept>
#include <set>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <common_utils/log/Logger.hpp>

#include "BluetoothDevice.hpp"

namespace carpi::bluetooth {
    class BluetoothManager {
        LOGGER;

        int32_t _device_id = 0;
        int32_t _socket = 0;

        static std::string error_to_string();

    public:
        BluetoothManager();

        std::set<BluetoothDevice> scan_devices();
    };
}

#endif