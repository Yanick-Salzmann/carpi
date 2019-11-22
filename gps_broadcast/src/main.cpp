#include <iostream>
#include <gps/gps_listener.hpp>
#include "net_broadcast.hpp"
#include <csignal>

namespace carpi {
    gps::GpsListener* gps_listener = nullptr;

    void signal_handler(int signal) {
        if(gps_listener != nullptr) {
            gps_listener->stop_gps_loop();
        }
    }

    int main(int argc, char* argv[]) {
        gps_listener = new gps::GpsListener{};

        gps::NetBroadcast bcast{};
        gps_listener->data_callback([&](const auto& m) { bcast.on_measurement(m); });

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        gps_listener->start_gps_loop(true);
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}