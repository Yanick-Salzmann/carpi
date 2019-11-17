#include "bluetooth_utils/low_energy.hpp"
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdexcept>
#include <unistd.h>

namespace carpi::bluetooth {

    GattServer::GattServer() {
        _socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        const auto device_id = hci_get_route(nullptr);
        const auto hci_socket = hci_open_dev(device_id);

        sockaddr_l2 l2_addr{
                .l2_family = AF_BLUETOOTH,
                .l2_bdaddr = bdaddr_t{{0, 0, 0, 0, 0, 0}},
                .l2_cid = htobs(4)
        };

        auto rc = bind(_socket, (const sockaddr*) &l2_addr, sizeof l2_addr);
        if(rc < 0) {
            throw std::runtime_error{"Error binding HCI socket"};
        }

        rc = listen(_socket, 1);
        if(rc < 0) {
            throw std::runtime_error{"Error listening on HCI socket"};
        }

        while(1) {
            socklen_t len = sizeof l2_addr;
            auto client = accept(_socket, (sockaddr*) &l2_addr, &len);
            if(client < 0) {
                throw std::runtime_error{"Error accepting"};
            }

            char buffer[4096]{0};
            int ret = read(client, buffer, sizeof(buffer));
            printf("data len: %d\n", ret);
            for (auto i = 0; i < ret; i++) {
                printf("%02x", ((int)buffer[i]) & 0xff);
            }
            printf("\n");
            close(client);
        }
    }
}