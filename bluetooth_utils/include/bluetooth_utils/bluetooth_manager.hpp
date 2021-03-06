#ifndef CARPI_BLUETOOTH_BLUETOOTH_MANAGER_H
#define CARPI_BLUETOOTH_BLUETOOTH_MANAGER_H

#include <string>
#include <stdexcept>
#include <set>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <common_utils/log.hpp>
#include <chrono>
#include <common_utils/singleton.hpp>

#include "bluetooth_device.hpp"

namespace carpi::bluetooth {
    class BluetoothManager : public utils::singleton<BluetoothManager> {
        LOGGER;

        int32_t _device_id = 0;
        int32_t _socket = 0;

    public:
        BluetoothManager();

        std::set<BluetoothDevice> scan_devices(uint32_t search_ticks = 1);

        template<typename Rep, typename Period>
        std::set<BluetoothDevice> scan_devices(const std::chrono::duration<Rep, Period> &max_scan_duration) {
            const auto ticks = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(max_scan_duration).count() / 1280.0f);
            return scan_devices(std::max(ticks, 1u));
        }

        BluetoothDevice open_device(const std::string& address);
    };
}

#define sBluetoothMgr (carpi::bluetooth::BluetoothManager::instance())

#endif