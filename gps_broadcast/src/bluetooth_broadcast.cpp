#include <common_utils/string.hpp>
#include <bluetooth_utils/bluetooth_device.hpp>
#include <bluetooth_utils/bluetooth_server.hpp>
#include "bluetooth_broadcast.hpp"

namespace carpi::gps {
    LOGGER_IMPL(BluetoothBroadcast);

    BluetoothBroadcast::BluetoothBroadcast(const std::string &mode, std::string target_device, uint8_t channel) {
        const auto lower_mode = utils::to_lower(mode);
        if (lower_mode == "client") {
            init_client_mode(target_device, channel);
        } else {
            if (lower_mode != "server") {
                log->warn("Invalid bluetooth broadcast mode '{}'. Must be 'client' or 'server'. Defaulting to 'server'", lower_mode);
            }

            init_server_mode(channel);
        }
    }

    BluetoothBroadcast::~BluetoothBroadcast() {
        _is_running = false;
        if(_server) {
            _server->close();
        }

        if (!_is_active_mode && _listener_thread.joinable()) {
            _listener_thread.join();
        }
    }

    void BluetoothBroadcast::init_server_mode(uint8_t channel) {
        log->info("Initializing passive bluetooth broadcast");
        _is_active_mode = false;
        _server = std::make_shared<bluetooth::BluetoothServer>(channel);

        _listener_thread = std::thread{
            [this]() {
                passive_mode();
            }
        };
    }

    void BluetoothBroadcast::init_client_mode(std::string device_address, uint8_t channel) {
        log->info("Initializing active bluetooth broadcast");
        _is_active_mode = true;

        _client_connection = bluetooth::BluetoothDevice::open_device(device_address)->connect(channel);
        log->info("Established remote connection to {}", *_client_connection);
    }

    void BluetoothBroadcast::send_packet(const ipc::IpcPackage &package) {
        if (_is_active_mode) {
            if (_client_connection) {
                std::vector<uint8_t> buffer{};
                package.to_buffer(buffer);
                _client_connection->write_data(buffer.data(), buffer.size());
            }
        } else {
            std::vector<uint8_t> buffer{};
            package.to_buffer(buffer);

            std::lock_guard<std::mutex> l{_connection_lock};
            std::list<BtConnPtr> conns{_server_connections};
            for (auto &conn : conns) {
                if (!conn->write_data(buffer.data(), buffer.size())) {
                    _server_connections.remove(conn);
                }
            }
        }
    }

    void BluetoothBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        log->info("Sending measurement: {}, {}, {}", (float) gps_data.lat, (float) gps_data.lon, (float) gps_data.alt);
        ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
        package << gps_data.fix << gps_data.lat << gps_data.lon << gps_data.alt;
        send_packet(package);
    }

    void BluetoothBroadcast::passive_mode() {
        while (_is_running) {
            const auto conn = _server->accept_connection();
            if(conn == nullptr) {
                break;
            }

            log->info("Accepted new bluetooth broadcast connection: {}", conn->to_string());
            {
                std::lock_guard<std::mutex> l{_connection_lock};
                _server_connections.emplace_back(conn);
            }
        }
    }
}