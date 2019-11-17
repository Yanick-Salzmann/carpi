#ifndef CARPI_GPS_GPS_LISTENER_HPP
#define CARPI_GPS_GPS_LISTENER_HPP

#include <gps.h>
#include <string>

namespace carpi::gps {
    class GpsListener {
    public:
        explicit GpsListener(const std::string& host = "localhost", uint16_t port = 2947);
    };
}

#endif //CARPI_GPS_GPS_LISTENER_HPP
