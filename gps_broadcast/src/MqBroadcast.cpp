#include "MqBroadcast.hpp"

namespace carpi::gps {
    LOGGER_IMPL(MqBroadcast);

    MqBroadcast::MqBroadcast() : _message_queue{ipc::IpcMessageQueue::GPS_BROADCAST} {

    }

    void MqBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        log->info("GPS: {}/{}/{}", gps_data.lat, gps_data.lon, gps_data.alt);
        ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
        package << gps_data.lat << gps_data.lon << gps_data.alt;
        _message_queue.send(package);
    }
}