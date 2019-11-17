#include "gps_event_listener.hpp"

namespace carpi {
    LOGGER_IMPL(GpsEventListener);

    GpsEventListener::GpsEventListener() : _message_queue{ipc::IpcMessageQueue::GPS_BROADCAST} {

    }

    void GpsEventListener::gps_message_thread() {
        while(_is_running) {
            auto pkg = _message_queue.receive();
            log->info("Received MQ package");
        }
    }

    void GpsEventListener::shutdown() {
        if(!_is_running) {
            return;
        }

        _is_running = false;
        if(_gps_thread.joinable()) {
            _gps_thread.join();
        }
    }

    void GpsEventListener::run() {
        if(_is_running) {
            return;
        }

        _is_running = true;
        _gps_thread = std::thread{[=]() { gps_message_thread(); }};
    }
}