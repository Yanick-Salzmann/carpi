#ifndef CARPI_GPS_BCAST_NET_BROADCAST_HPP
#define CARPI_GPS_BCAST_NET_BROADCAST_HPP

#include <common_utils/log.hpp>
#include <netinet/in.h>
#include <ipc_common/ipc_package.hpp>
#include <gps/gps_listener.hpp>

namespace carpi::gps {
    class NetBroadcast {
        LOGGER;

        int _socket = 0;

        sockaddr_in _ip4_broadcast{};
        sockaddr_in6 _ip6_broadcast{};
        sa_family_t _address_family = AF_INET;

    public:
        NetBroadcast();

        void send_packet(const ipc::IpcPackage& packet);

        void on_measurement(const GpsMeasurement& measurement);
    };
}

#endif //CARPI_GPS_BCAST_NET_BROADCAST_HPP
