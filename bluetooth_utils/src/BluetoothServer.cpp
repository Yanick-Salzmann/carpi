#include "bluetooth_utils/BluetoothServer.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"
#include "common_utils/error.hpp"

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothServer);

    BluetoothServer::BluetoothServer(uint16_t psm, uint16_t cid) {
        _socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (_socket < 0) {
            log->error("Error creating bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error creating bluetooth socket");
        }

        bdaddr_t any_addr{{0, 0, 0, 0, 0, 0}};

        sockaddr_l2 loc_addr{};
        loc_addr.l2_family = AF_BLUETOOTH;
        bacpy(&loc_addr.l2_bdaddr, &any_addr);
        loc_addr.l2_cid = htobs(cid);
        loc_addr.l2_psm = htobs(psm);

        auto rc = bind(_socket, (const sockaddr*) &loc_addr, sizeof loc_addr);
        if(rc < 0) {
            log->error("Error binding bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error binding bluetooth socket");
        }

        rc = listen(_socket, 1);
        if(rc < 0) {
            log->error("Error listening on bluetooth socket: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error listening on bluetooth socket");
        }

        socklen_t addr_size = sizeof loc_addr;
        getsockname(_socket, (sockaddr*) &loc_addr, &addr_size);
        char addr_name[18]{};
        ba2str(&loc_addr.l2_bdaddr, addr_name);
        addr_name[17] = '\0';

        log->debug("Bluetooth server listening on socket {X}, addr={}, psm={}, cid={}", _socket, addr_name, psm, cid);
    }

    std::shared_ptr<BluetoothConnection> BluetoothServer::accept_connection() {
        sockaddr_l2 client_addr{};
        socklen_t addr_size = sizeof client_addr;

        const auto client = accept(_socket, (sockaddr*) &client_addr, &addr_size);
        if(client < 0) {
            log->error("Error accepting bluetooth client: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error accepting bluetooth client");
        }

        char addr_name[18]{};
        ba2str(&client_addr.l2_bdaddr, addr_name);
        addr_name[17] = '\0';

        log->debug("Accepted bluetooth client from {}", addr_name);
        return std::make_shared<BluetoothConnection>(client);
    }
}