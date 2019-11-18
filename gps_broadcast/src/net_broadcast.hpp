#ifndef CARPI_GPS_BCAST_NET_BROADCAST_HPP
#define CARPI_GPS_BCAST_NET_BROADCAST_HPP

#include <common_utils/log.hpp>
#include <ipc_common/ipc_package.hpp>
#include <gps/gps_listener.hpp>
#include <net_utils/udp_broadcast.hpp>
#include <memory>

namespace carpi::gps {
    class NetBroadcast {
        LOGGER;

        std::shared_ptr<net::UdpBroadcast> _broadcast;

        void send_packet(const ipc::IpcPackage& packet);

    public:
        NetBroadcast();

        void on_measurement(const GpsMeasurement& measurement);
    };
}

#endif //CARPI_GPS_BCAST_NET_BROADCAST_HPP
