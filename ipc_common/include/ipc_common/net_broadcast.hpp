#ifndef CARPI_IPC_UTILS_NET_BROADCAST_HPP
#define CARPI_IPC_UTILS_NET_BROADCAST_HPP

#include <common_utils/log.hpp>
#include <net_utils/udp_multicast.hpp>
#include "ipc_package.hpp"

namespace carpi::ipc {
    class NetBroadcast {
        LOGGER;

        std::shared_ptr<net::UdpMulticast> _broadcast;

    public:
        NetBroadcast(std::string address, uint16_t port);
        void send_packet(const ipc::IpcPackage& packet);
    };
}

#endif //CARPI_IPC_UTILS_NET_BROADCAST_HPP
