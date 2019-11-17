#ifndef CARPI_GPS_BCAST_MQBROADCAST_HPP
#define CARPI_GPS_BCAST_MQBROADCAST_HPP

#include <gps/gps_listener.hpp>
#include <common_utils/log.hpp>

namespace carpi::gps {
    class MqBroadcast {
        LOGGER;

    public:
        void on_measurement(const GpsMeasurement& gps_data);
    };
}

#endif //CARPI_GPS_BCAST_MQBROADCAST_HPP
