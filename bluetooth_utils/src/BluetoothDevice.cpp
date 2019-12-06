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

        if(hci_read_remote_name(_socket, &address, sizeof device_name, device_name, 0) == 0) {
            device_name[247] = '\0';
            _device_name.assign(device_name);
        } else {
            _device_name.assign("Unknown Device");
        }
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

    std::shared_ptr<BluetoothDevice> BluetoothDevice::open_device(const std::string &address) {
        bdaddr_t bt_addr{};
        str2ba(address.c_str(), &bt_addr);

        const auto route = hci_get_route(nullptr);
        if(route < 0) {
            log->error("Error opening bluetooth device: Cannot get route: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error{"Error opening route to BT device"};
        }

        const auto socket = hci_open_dev(route);
        if(socket < 0) {
            log->error("Error opening bluetooth device: Cannot open socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error{"Error opening socket to BT device"};
        }

        return std::make_shared<BluetoothDevice>(socket, bt_addr);
    }
}