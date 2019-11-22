#ifndef CARPI_GPS_GPS_LISTENER_HPP
#define CARPI_GPS_GPS_LISTENER_HPP

#include <gps.h>
#include <string>
#include <common_utils/log.hpp>
#include <functional>

namespace carpi::gps {
    struct GpsMeasurement {
        double lat;
        double lon;
        double alt;
    };

    class GpsListener {
        LOGGER;

        bool _is_in_main_thread = false;
        bool _is_running = false;
        std::thread _gps_thread;
        gps_data_t _gps_data{};
        std::mutex _callback_lock;

        std::function<void (const GpsMeasurement&)> _callback;

        void gps_loop();

    public:
        explicit GpsListener(const std::string& host = "localhost", uint16_t port = 2947);
        ~GpsListener();

        void start_gps_loop(bool main_thread = false);
        void stop_gps_loop();

        void data_callback(std::function<void(const GpsMeasurement&)> callback) {
            std::lock_guard<std::mutex> l{_callback_lock};
            _callback = std::move(callback);
        }
    };
}

#endif //CARPI_GPS_GPS_LISTENER_HPP
