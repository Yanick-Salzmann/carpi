#ifndef CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP
#define CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP

#include <string>
#include <memory>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "common_utils/log.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace carpi::bluetooth {
    class BluetoothConnection;

    class BluetoothDevice {
        LOGGER;

        bdaddr_t _device_address;
        int32_t _socket;

        std::string _device_name;
        std::string _address_string;

    public:
        explicit BluetoothDevice(int32_t socket, bdaddr_t address);

        std::shared_ptr<BluetoothConnection> connect(uint8_t channel) const;

        bool operator<(const BluetoothDevice &other) const;

        bool operator==(const BluetoothDevice &other) const;

        bool operator!=(const BluetoothDevice &other) const;

        [[nodiscard]] std::string device_name() const {
            return _device_name;
        }

        [[nodiscard]] std::string address_string() const {
            return _address_string;
        }

        [[nodiscard]] std::string to_string() const {
            return fmt::format("<BluetoothDevice name='{}', address='{}'>", _device_name, _address_string);
        }

        template<typename OStream>
        friend OStream& operator << (OStream& os, const BluetoothDevice& device) {
            return os << device.to_string();
        }
    };
}

#endif //CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP
