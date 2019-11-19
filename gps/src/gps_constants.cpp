#include "gps/gps_constants.hpp"

namespace carpi::gps {
    static std::string GPS_MULTICAST_INTERFACE{"239.255.255.255"};

    std::string gps_multicast_interface() {
        return GPS_MULTICAST_INTERFACE;
    }
}