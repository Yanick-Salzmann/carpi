#include <iostream>
#include <gps/gps_listener.hpp>
#include "ipc_common/net_broadcast.hpp"
#include "ipc_common/bluetooth_broadcast.hpp"
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

        std::shared_ptr<ipc::NetBroadcast> udp_bcast{};
        std::shared_ptr<ipc::BluetoothBroadcast> bt_bcast{};

        if (toml::find_or<bool>(udp_conf, "enabled", false)) {
            udp_bcast = std::make_shared<ipc::NetBroadcast>(toml::find<std::string>(udp_conf, "address"), toml::find<uint16_t>(udp_conf, "port"));
            gps_listener->data_callback([udp_bcast](const auto &m) {
                ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
                package << m.fix << m.lat << m.lon << m.alt;
                udp_bcast->send_packet(package);
            });
        }

        if (toml::find_or<bool>(btconf, "enabled", false)) {
            bt_bcast = std::make_shared<ipc::BluetoothBroadcast>(
                    toml::find<std::string>(btconf, "mode"),
                    toml::find_or<std::string>(btconf, "target", ""),
                    toml::find<uint8_t>(btconf, "channel")
            );

            gps_listener->data_callback([&](const auto &m) {
                ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
                package << m.fix << m.lat << m.lon << m.alt;
                bt_bcast->send_packet(package);
            });
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