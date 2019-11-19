#ifndef CARPI_CARPI_GPS_LISTENER_THREAD_HPP
#define CARPI_CARPI_GPS_LISTENER_THREAD_HPP

#include <memory>
#include <net_utils/udp_multicast.hpp>
#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>
#include <gps/gps_listener.hpp>

namespace carpi {
    class GpsListenerThread : public utils::Singleton<GpsListenerThread> {
        LOGGER;

        std::shared_ptr<net::UdpMulticast> _multicast;

        bool _is_running = false;
        std::thread _gps_loop{};

        std::mutex _measure_lock;
        gps::GpsMeasurement _active_measurement;

        void start_loop();

        void gps_loop();

    public:
        GpsListenerThread();

        void start() {
            start_loop();
        }

        void stop();

        void active_measurement(gps::GpsMeasurement& measurement);
    };
}

#define sGpsListener (carpi::GpsListenerThread::instance())

#endif //CARPI_CARPI_GPS_LISTENER_THREAD_HPP
