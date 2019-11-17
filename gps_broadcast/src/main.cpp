#include <iostream>
#include <gps/gps_listener.hpp>
#include "MqBroadcast.hpp"

namespace carpi {
    int main(int argc, char* argv[]) {
        gps::GpsListener listener{};
        listener.start_gps_loop();

        gps::MqBroadcast broadcast{};

        listener.data_callback(std::bind(&gps::MqBroadcast::on_measurement, &broadcast, std::placeholders::_1));

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