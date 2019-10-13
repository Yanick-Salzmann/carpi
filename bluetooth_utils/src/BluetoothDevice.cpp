#include "bluetooth_utils/BluetoothDevice.hpp"

namespace carpi::bluetooth {
    BluetoothDevice::BluetoothDevice(int32_t socket, bdaddr_t address) : _socket(socket), _device_address(address) {
        char device_name[248]{};

        if(hci_read_remote_name(socket, &address, sizeof device_name, device_name, 0)) {
            device_name[247] = '\0';
            _device_name.assign(device_name);
        } else {
            _device_name.assign("Unknown Device");
        }

        char device_address[18]{};
        ba2str(&address, device_address);
        device_address[17] = '\0';
        _address_string.assign(device_address);
    }

    bool BluetoothDevice::operator<(const BluetoothDevice &other) const {
        if(_socket != other._socket) {
            return _socket < other._socket;
        }

        return memcmp(&_device_address, &other._device_address, sizeof(bdaddr_t)) < 0;
    }

    bool BluetoothDevice::operator==(const BluetoothDevice &other) const {
        return _socket == other._socket && memcmp(&_device_address, &other._device_address, sizeof(bdaddr_t)) == 0;
    }

    bool BluetoothDevice::operator!=(const BluetoothDevice &other) const {
        return !(*this == other);
    }
}