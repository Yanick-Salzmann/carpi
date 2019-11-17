#ifndef CARPI_GPS_BCAST_NET_BROADCAST_HPP
#define CARPI_GPS_BCAST_NET_BROADCAST_HPP

#include <common_utils/log.hpp>

namespace carpi::gps {
    class NetBroadcast {
        LOGGER;

        int _socket = 0;

    public:
        NetBroadcast();
    };
}

#endif //CARPI_GPS_BCAST_NET_BROADCAST_HPP
