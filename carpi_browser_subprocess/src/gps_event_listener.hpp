#ifndef CARPI_CARPI_SUBPROC_GPS_EVENT_LISTENER_HPP
#define CARPI_CARPI_SUBPROC_GPS_EVENT_LISTENER_HPP

#include <ipc_common/ipc_mq.hpp>
#include <common_utils/log.hpp>
#include <common_utils/singleton.hpp>

namespace carpi {
    class GpsEventListener : public utils::Singleton<GpsEventListener> {
        LOGGER;

        ipc::MessageQueue _message_queue;

        std::thread _gps_thread;
        bool _is_running = false;

        void gps_message_thread();

    public:
        GpsEventListener();

        ~GpsEventListener() {
            shutdown();
        }

        void shutdown();

        void run();
    };
}

#define sGpsEvtLstnr (carpi::GpsEventListener::instance())

#endif //CARPI_CARPI_SUBPROC_GPS_EVENT_LISTENER_HPP
