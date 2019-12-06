#include <iostream>
#include <gps/gps_listener.hpp>
#include "net_broadcast.hpp"
#include <csignal>
#include <toml.hpp>

namespace carpi {
    gps::GpsListener *gps_listener = nullptr;

    void signal_handler(int) {
        if (gps_listener != nullptr) {
            gps_listener->stop_gps_loop();
        }
    }

    int main() {
        {
            std::ofstream os{"app.pid"};
            os << getpid();
        }

        const auto config = toml::parse("resources/config.toml");

        gps_listener = new gps::GpsListener{};

        const auto gps_conf = toml::find(config, "gps");
        const auto bcst_conf = toml::find(gps_conf, "broadcast");
        const auto udp_conf = toml::find(bcst_conf, "udp");

        if (toml::find_or<bool>(udp_conf, "enabled", false)) {
            gps::NetBroadcast bcast{toml::find<std::string>(udp_conf, "address"), toml::find<uint16_t>(udp_conf, "port")};
            gps_listener->data_callback([&](const auto &m) { bcast.on_measurement(m); });
        }

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        gps_listener->start_gps_loop(true);
        return 0;
    }
}

int main(int argc, char *argv[]) {
    return carpi::main();
}