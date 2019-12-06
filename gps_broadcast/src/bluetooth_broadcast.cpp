#include <common_utils/string.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include "bluetooth_broadcast.hpp"

namespace carpi::gps {
    LOGGER_IMPL(BluetoothBroadcast);

    BluetoothBroadcast::BluetoothBroadcast(const std::string &mode, std::string target_device) {
        const auto lower_mode = utils::to_lower(mode);
        if(lower_mode == "client") {
            init_client_mode(target_device);
        } else {
            if(lower_mode != "server") {
                log->warn("Invalid bluetooth broadcast mode '{}'. Must be 'client' or 'server'. Defaulting to 'server'", lower_mode);
            }

            init_server_mode();
        }
    }

    void BluetoothBroadcast::init_server_mode() {
        log->info("Initializing passive bluetooth broadcast");
        _is_active_mode = false;
    }

    void BluetoothBroadcast::init_client_mode(std::string device_address) {
        log->info("Initializing active bluetooth broadcast");
        _is_active_mode = true;

        _client_connection = bluetooth::BluetoothDevice::open_device(device_address)->connect(1);
        log->info("Established remote connection to {}", *_client_connection);
    }

    void BluetoothBroadcast::send_packet(const ipc::IpcPackage &package) {
        if(_is_active_mode) {
            if(_client_connection) {
                std::vector<uint8_t> buffer{};
                package.to_buffer(buffer);
                _client_connection->write_data(buffer.data(), buffer.size());
            }
        } else {
            std::vector<uint8_t> buffer{};
            package.to_buffer(buffer);

            std::lock_guard<std::mutex> l{_connection_lock};
            std::list<BtConnPtr> conns{_server_connections};
            for(auto& conn : conns) {
                if(!conn->write_data(buffer.data(), buffer.size())) {
                    _server_connections.remove(conn);
                }
            }
        }
    }

    void BluetoothBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
        package << gps_data.fix << gps_data.lat << gps_data.lon << gps_data.alt;
        send_packet(package);
    }
}