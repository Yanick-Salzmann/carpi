#ifndef CARPI_GPS_GPS_LISTENER_HPP
#define CARPI_GPS_GPS_LISTENER_HPP

#include <gps.h>
#include <string>
#include <common_utils/log.hpp>
#include <functional>
#include <deque>

namespace carpi::gps {
#pragma pack(push, 1)
    struct GpsMeasurement {
        bool fix;
        double lat;
        double lon;
        double alt;
    };
#pragma pack(pop)

    class GpsListener {
        LOGGER;

        bool _is_in_main_thread = false;
        bool _is_running = false;
        std::thread _gps_thread;
        gps_data_t _gps_data{};
        std::mutex _callback_lock;

        std::deque<std::function<void (const GpsMeasurement&)>> _callbacks;

        void gps_loop();

    public:
        explicit GpsListener(const std::string& host = "localhost", uint16_t port = 2947);
        ~GpsListener();

        void start_gps_loop(bool main_thread = false);
        void stop_gps_loop();

        void data_callback(std::function<void(const GpsMeasurement&)> callback) {
            std::lock_guard<std::mutex> l{_callback_lock};
            _callbacks.emplace_back(std::move(callback));
        }
    };
}

#endif //CARPI_GPS_GPS_LISTENER_HPP
