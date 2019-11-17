#ifndef CARPI_GPS_GPS_LISTENER_HPP
#define CARPI_GPS_GPS_LISTENER_HPP

#include <gps.h>
#include <string>
#include <common_utils/log.hpp>

namespace carpi::gps {
    class GpsListener {
        LOGGER;

    public:
        explicit GpsListener(const std::string& host = "localhost", uint16_t port = 2947);
    };
}

#endif //CARPI_GPS_GPS_LISTENER_HPP
