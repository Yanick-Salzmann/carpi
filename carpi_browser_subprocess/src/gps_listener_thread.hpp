#ifndef CARPI_CARPI_GPS_LISTENER_THREAD_HPP
#define CARPI_CARPI_GPS_LISTENER_THREAD_HPP

#include <memory>
#include <net_utils/udp_multicast.hpp>
#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>
#include <gps/gps_listener.hpp>
#include <bluetooth_utils/bluetooth_connection.hpp>

namespace carpi {
    class GpsListenerThread : public utils::Singleton<GpsListenerThread> {
        LOGGER;

        std::shared_ptr<net::UdpMulticast> _multicast;
        std::shared_ptr<bluetooth::BluetoothConnection> _bluetooth_connection;

        bool _is_bluetooth_mode = false;
        bool _is_running = false;
        std::thread _gps_loop{};
        std::thread _bt_loop{};

        std::mutex _measure_lock;
        gps::GpsMeasurement _active_measurement;

        void start_loop();

        void gps_loop();
        void bluetooth_loop();

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
