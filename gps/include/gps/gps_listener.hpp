#ifndef CARPI_GPS_GPS_LISTENER_HPP
#define CARPI_GPS_GPS_LISTENER_HPP

#include <gps.h>
#include <string>
#include <common_utils/log.hpp>

namespace carpi::gps {
    class GpsListener {
        LOGGER;

        bool _is_running = false;
        std::thread _gps_thread;
        gps_data_t _gps_data{};

        void gps_loop();

    public:
        explicit GpsListener(const std::string& host = "localhost", uint16_t port = 2947);
        ~GpsListener();

        void start_gps_loop();
        void stop_gps_loop();
    };
}

#endif //CARPI_GPS_GPS_LISTENER_HPP
