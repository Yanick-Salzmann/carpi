#include <iostream>
#include <gps/gps_listener.hpp>
#include "net_broadcast.hpp"

namespace carpi {
    int main(int argc, char* argv[]) {
        gps::GpsListener listener{};
        listener.start_gps_loop();

        gps::NetBroadcast bcast{};
        listener.data_callback([&](const auto& m) { bcast.on_measurement(m); });

        std::cin.sync();
        std::string line{};
        std::getline(std::cin, line);

        listener.stop_gps_loop();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}