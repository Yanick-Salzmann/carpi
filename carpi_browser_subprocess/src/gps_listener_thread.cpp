#include "gps_listener_thread.hpp"
#include <gps/gps_constants.hpp>
#include <common_utils/error.hpp>
#include <toml.hpp>
#include <common_utils/string.hpp>
#include <bluetooth_utils/bluetooth_device.hpp>

namespace carpi {
    LOGGER_IMPL(GpsListenerThread);

    GpsListenerThread::GpsListenerThread() {
        const auto cfg = toml::parse("resources/config.toml");
        const auto gps_cfg = toml::find(cfg, "gps");
        const auto gps_data_cfg = toml::find(gps_cfg, "data");

        const auto mode = utils::to_lower(toml::find<std::string>(gps_data_cfg, "mode"));
        log->info("GPS listening mode: {}", mode);
        if (mode == "bluetooth") {
            _is_bluetooth_mode = true;
            const auto bt_cfg = toml::find(gps_data_cfg, "bluetooth");
            std::thread{
                [bt_cfg, this]() {
                    log->info("Opening bluetooth connection: {}", toml::find<std::string>(bt_cfg, "source"));
                    _bluetooth_connection = bluetooth::BluetoothDevice::open_device(toml::find<std::string>(bt_cfg, "source"))->connect(toml::find<uint8_t>(bt_cfg, "channel"));
                    log->info("Opened bluetooth connection: {}", _bluetooth_connection->to_string());
                    _is_initialized = true;
                }
            }.detach();

        } else {
            _is_bluetooth_mode = false;
            const auto udp_cfg = toml::find(gps_data_cfg, "udp");
            const auto bcast_address = toml::find<std::string>(udp_cfg, "address");
            const auto bcast_port = toml::find<uint16_t>(udp_cfg, "port");
            _multicast = std::make_shared<net::UdpMulticast>(bcast_address, bcast_port, true);
        }
    }

    void GpsListenerThread::start_loop() {
        if (_is_running) {
            return;
        }

        _is_running = true;
        if (_is_bluetooth_mode) {
            _bt_loop = std::thread{[=]() { bluetooth_loop(); }};
        } else {
            _gps_loop = std::thread{[=]() { gps_loop(); }};
        }
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
        if (_multicast) {
            _multicast->close();
        }

        if (_bluetooth_connection) {
            _bluetooth_connection->close();
        }

        if (_gps_loop.joinable()) {
            _gps_loop.join();
        }

        if (_bt_loop.joinable()) {
            _bt_loop.join();
        }
    }

    void GpsListenerThread::active_measurement(gps::GpsMeasurement &m) {
        std::lock_guard<std::mutex> l{_measure_lock};
        m = _active_measurement;
    }

    void GpsListenerThread::bluetooth_loop() {
        uint8_t gps_buffer[8 + sizeof(gps::GpsMeasurement)]{};
        while(_is_running) {
            if(!_is_initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
                continue;
            }

            if(!_bluetooth_connection->read_data(gps_buffer, sizeof gps_buffer)) {
                log->error("Error receiving GPS bluetooth data: {} (errno={})", utils::error_to_string(errno), errno);
                continue;
            }

            const auto measurement = *(const gps::GpsMeasurement *) (gps_buffer + 8);
            {
                std::lock_guard<std::mutex> l{_measure_lock};
                _active_measurement = measurement;
            }
        }
    }
}