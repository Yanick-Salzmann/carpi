#include "bluetooth_utils/BluetoothDevice.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"

#include <bluetooth/l2cap.h>
#include <common_utils/error.hpp>

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothDevice);

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

    std::shared_ptr<BluetoothConnection> BluetoothDevice::connect(uint16_t psm, uint16_t cid) {
        const auto ret_client = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if(ret_client < 0) {
            log->error("Error creating client socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error creating client socket");
        }

        sockaddr_l2 l2_addr{};
        l2_addr.l2_bdaddr_type = AF_BLUETOOTH;
        bacpy(&l2_addr.l2_bdaddr, &_device_address);
        const auto rc = ::connect(ret_client, (const sockaddr*) &l2_addr, sizeof l2_addr);
        if(rc < 0) {
            log->error("Error opening connection to device {} with psm={} and cid={}: {} (errno={})", _address_string, psm, cid, utils::error_to_string(), errno);
            throw std::runtime_error("Error opening connection to device");
        }

        socklen_t addr_size = sizeof l2_addr;
        getsockname(ret_client, (sockaddr*) &l2_addr, &addr_size);

        char remote_addr[18]{};
        ba2str(&l2_addr.l2_bdaddr, remote_addr);
        remote_addr[17] = '\0';

        log->debug("Created bluetooth connection to {} with psm={} and cid={}", remote_addr, psm, cid);

        return std::make_shared<BluetoothConnection>(ret_client);
    }
}