#include <common_utils/error.hpp>
#include "gps/gps_listener.hpp"

namespace carpi::gps {
    LOGGER_IMPL(GpsListener);

    GpsListener::GpsListener(const std::string &host, uint16_t port) {
        auto rc = gps_open(host.c_str(), std::to_string(port).c_str(), &_gps_data);
        if (rc < 0) {
            log->error("Error opening GPS socket: {} (errno={})", gps_errstr(rc), rc);
            throw std::runtime_error{"Error opening GPS socket"};
        }

        log->info("Opened socket to gpsd");

        rc = gps_stream(&_gps_data, WATCH_ENABLE | WATCH_JSON, nullptr);
        if (rc < 0) {
            log->error("Error starting GPS stream: {} (errno={})", gps_errstr(rc), rc);
            throw std::runtime_error{"Error streaming GPS"};
        }
    }

    GpsListener::~GpsListener() {
        stop_gps_loop();

        gps_stream(&_gps_data, WATCH_DISABLE, nullptr);
        gps_close(&_gps_data);
    }

    void GpsListener::start_gps_loop() {
        if (_is_running) {
            return;
        }

        log->info("Starting GPS loop");

        _is_running = true;
        _gps_thread = std::thread{[=]() { gps_loop(); }};
    }

    void GpsListener::stop_gps_loop() {
        if (!_is_running) {
            return;
        }

        _is_running = false;
        if (_gps_thread.joinable()) {
            _gps_thread.join();
        }
    }

    void GpsListener::gps_loop() {
        while (_is_running) {
            if (gps_waiting(&_gps_data, 10000)) {
                const auto rc = gps_read(&_gps_data);
                if (rc < 0) {
                    log->error("Error reading GPS data: {} (errno={})", gps_errstr(rc), rc);
                    continue;
                }

                if (_gps_data.status == STATUS_FIX && (_gps_data.fix.mode == MODE_2D || _gps_data.fix.mode == MODE_3D) &&
                    !std::isnan(_gps_data.fix.latitude) && !std::isnan(_gps_data.fix.longitude)) {
                    GpsMeasurement ms{
                            .lat = _gps_data.fix.latitude,
                            .lon = _gps_data.fix.longitude,
                            .alt = _gps_data.fix.altitude
                    };

                    {
                        std::lock_guard<std::mutex> l{_callback_lock};
                        if (_callback) {
                            _callback(ms);
                        }
                    }
                }
            }

            std::this_thread::yield();
        }
    }
}