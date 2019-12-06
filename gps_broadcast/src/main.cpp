#include <iostream>
#include <gps/gps_listener.hpp>
#include "net_broadcast.hpp"
#include "bluetooth_broadcast.hpp"
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
        const auto btconf = toml::find(bcst_conf, "bluetooth");

        std::shared_ptr<gps::NetBroadcast> udp_bcast{};

        if (toml::find_or<bool>(udp_conf, "enabled", false)) {
            udp_bcast = std::make_shared<gps::NetBroadcast>(toml::find<std::string>(udp_conf, "address"), toml::find<uint16_t>(udp_conf, "port"));
            gps_listener->data_callback([](const auto &m) { udp_bcast->on_measurement(m); });
        }

        if (toml::find_or<bool>(btconf, "enabled", false)) {
            gps::BluetoothBroadcast bcast{toml::find<std::string>(btconf, "mode"), toml::find_or<std::string>(btconf, "target", "")};
            gps_listener->data_callback([&](const auto& m) { });
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