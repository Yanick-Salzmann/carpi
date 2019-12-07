#include "gps_listener_thread.hpp"
#include <gps/gps_constants.hpp>
#include <common_utils/error.hpp>
#include <toml.hpp>
#include <common_utils/string.hpp>

namespace carpi {
    LOGGER_IMPL(GpsListenerThread);

    GpsListenerThread::GpsListenerThread() {
        const auto cfg = toml::parse("resources/config.toml");
        const auto gps_cfg = toml::find(cfg, "gps");
        const auto gps_data_cfg = toml::find(gps_cfg, "data");

        const auto mode = utils::to_lower(toml::find<std::string>(gps_data_cfg, "mode"));
        if(mode == "bluetooth") {
            const auto bt_cfg = toml::find(gps_data_cfg, "bluetooth");
        } else {
            const auto udp_cfg = toml::find(gps_data_cfg, "udp");
            _multicast = std::make_shared<net::UdpMulticast>(toml::find<std::string>(udp_cfg, "address"), toml::find<uint16_t>(udp_cfg, "port"), true);
        }
    }

    void GpsListenerThread::start_loop() {
        if (_is_running) {
            return;
        }

        _is_running = true;
        _gps_loop = std::thread{[=]() { gps_loop(); }};
    }

    void GpsListenerThread::gps_loop() {
        uint8_t gps_buffer[8 + sizeof(gps::GpsMeasurement)]{};

        while (_is_running) {
            if (_multicast->read_data(gps_buffer, sizeof gps_buffer) != sizeof gps_buffer) {
                log->warn("Error receiving GPS multicast data: {} (errno={})", utils::error_to_string(errno), errno);
                continue;
            }

            const auto measurement = *(const gps::GpsMeasurement *) (gps_buffer + 8);
            std::lock_guard<std::mutex> l{_measure_lock};
            _active_measurement = measurement;
        }
    }

    void GpsListenerThread::stop() {
        _is_running = false;
        _multicast->close();
        if (_gps_loop.joinable()) {
            _gps_loop.join();
        }
    }

    void GpsListenerThread::active_measurement(gps::GpsMeasurement &m) {
        std::lock_guard<std::mutex> l{_measure_lock};
        m = _active_measurement;
    }
}