#ifndef CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP
#define CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP

#include <string>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

namespace carpi::bluetooth {
    class BluetoothDevice {
        bdaddr_t _device_address;
        int32_t _socket;

        std::string _device_name;
        std::string _address_string;

    public:
        explicit BluetoothDevice(int32_t socket, bdaddr_t address);

        bool operator<(const BluetoothDevice &other) const;

        bool operator==(const BluetoothDevice &other) const;

        bool operator!=(const BluetoothDevice &other) const;

        [[nodiscard]] std::string device_name() const {
            return _device_name;
        }

        [[nodiscard]] std::string address_string() const {
            return _address_string;
        }
    };
}

#endif //CARPI_BLUETOOTH_UTILS_BLUETOOTHDEVICE_HPP
