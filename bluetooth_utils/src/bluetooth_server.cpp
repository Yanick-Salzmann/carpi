#include "bluetooth_utils/bluetooth_server.hpp"
#include "bluetooth_utils/bluetooth_connection.hpp"
#include "common_utils/error.hpp"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothServer);

    BluetoothServer::BluetoothServer(uint8_t channel) {
        _socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (_socket < 0) {
            log->error("Error creating bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error creating bluetooth socket");
        }

        bdaddr_t any_addr{{0, 0, 0, 0, 0, 0}};

        sockaddr_rc rc_addr{};
        rc_addr.rc_family = AF_BLUETOOTH;
        bacpy(&rc_addr.rc_bdaddr, &any_addr);
        rc_addr.rc_channel = channel;

        auto rc = bind(_socket, (const sockaddr*) &rc_addr, sizeof rc_addr);
        if(rc < 0) {
            log->error("Error binding bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error binding bluetooth socket");
        }

        rc = listen(_socket, 1);
        if(rc < 0) {
            log->error("Error listening on bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error listening on bluetooth socket");
        }

        socklen_t addr_size = sizeof rc_addr;
        getsockname(_socket, (sockaddr*) &rc_addr, &addr_size);
        char addr_name[18]{};
        ba2str(&rc_addr.rc_bdaddr, addr_name);
        addr_name[17] = '\0';

        log->debug("Bluetooth server listening on socket {:X}, addr={}, psm={}", _socket, addr_name, channel);
    }

    std::shared_ptr<BluetoothConnection> BluetoothServer::accept_connection() {
        sockaddr_rc client_addr{};
        socklen_t addr_size = sizeof client_addr;

        const auto client = accept(_socket, (sockaddr*) &client_addr, &addr_size);
        if(client < 0) {
            log->error("Error accepting bluetooth client: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error accepting bluetooth client");
        }

        char addr_name[18]{};
        ba2str(&client_addr.rc_bdaddr, addr_name);
        addr_name[17] = '\0';

        log->debug("Accepted bluetooth client from {}", addr_name);
        return std::make_shared<BluetoothConnection>(client, client_addr);
    }

    void BluetoothServer::close() {
        ::close(_socket);
    }
}