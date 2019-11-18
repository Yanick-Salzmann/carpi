#include "net_broadcast.hpp"
#include <common_utils/error.hpp>


namespace carpi::gps {
    LOGGER_IMPL(NetBroadcast);

    NetBroadcast::NetBroadcast() {
        _broadcast = std::make_shared<net::UdpBroadcast>(3377, false);
    }

    void NetBroadcast::send_packet(const ipc::IpcPackage &packet) {
        // int16_t is intentional, doesnt hurt to be smaller
        if (packet.size() >= std::numeric_limits<int16_t>::max()) {
            log->error("Packet is too large: {} >= {}", packet.size(), std::numeric_limits<int16_t>::max());
            throw std::runtime_error{"Packet too large"};
        }

        std::vector<uint8_t> data{};
        packet.to_buffer(data);

        const auto rc = _broadcast->send_data(data);
        if(rc < 0) {
            log->error("Error sending data: {} (errno={})", utils::error_to_string(errno), errno);
        }
    }

    void NetBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        log->info("GPS: {}/{}/{}", gps_data.lat, gps_data.lon, gps_data.alt);
        ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
        package << gps_data.lat << gps_data.lon << gps_data.alt;
        send_packet(package);
    }
}