#include "bluetooth_utils/BluetoothDevice.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"

#include <bluetooth/rfcomm.h>
#include <common_utils/error.hpp>

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothDevice);

    BluetoothDevice::BluetoothDevice(int32_t socket, bdaddr_t address) : _socket(socket), _device_address(address) {
        char device_address[18]{};
        ba2str(&address, device_address);
        device_address[17] = '\0';
        _address_string.assign(device_address);

        char device_name[248]{};

        /*const auto route = hci_get_route(&address);
        if(route < 0) {
            log->warn("No route to {} found: {} (errno={})", _address_string, utils::error_to_string(errno), errno);
            throw std::runtime_error{"No route to device found"};
        }

        const auto remote_socket = hci_open_dev(route);
        if(remote_socket < 0) {
            log->warn("Cannot open connection to {}: {} (errno={})", _address_string, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Cannot connect to device"};
        }*/

        if(hci_read_remote_name(_socket, &address, sizeof device_name, device_name, 0) == 0) {
            device_name[247] = '\0';
            _device_name.assign(device_name);
        } else {
            _device_name.assign("Unknown Device");
        }

        hci_close_dev(remote_socket);
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

    std::shared_ptr<BluetoothConnection> BluetoothDevice::connect(uint8_t channel) const {
        const auto ret_client = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if(ret_client < 0) {
            log->error("Error creating client socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error creating client socket");
        }

        sockaddr_rc rc_addr{};
        rc_addr.rc_family = AF_BLUETOOTH;
        bacpy(&rc_addr.rc_bdaddr, &_device_address);
        rc_addr.rc_channel = channel;

        const auto rc = ::connect(ret_client, (const sockaddr*) &rc_addr, sizeof rc_addr);
        if(rc < 0) {
            log->error("Error opening connection to device {} with channel={}: {} (errno={})", _address_string, channel, utils::error_to_string(), errno);
            throw std::runtime_error("Error opening connection to device");
        }

        log->debug("Created bluetooth connection to {} with channel={}", _address_string, channel);

        return std::make_shared<BluetoothConnection>(ret_client, rc_addr);
    }
}