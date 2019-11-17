#include "MqBroadcast.hpp"

namespace carpi::gps {
    LOGGER_IMPL(MqBroadcast);

    void MqBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        log->info("GPS: {}/{}/{}", gps_data.lat, gps_data.lon, gps_data.alt);
    }
}