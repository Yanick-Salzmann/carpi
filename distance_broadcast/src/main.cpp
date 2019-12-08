#include <iostream>
#include <csignal>

#include <common_utils/log.hpp>
#include <wiring_utils/serial_interface.hpp>
#include <wiring_utils/gpio.hpp>
#include <wiring_utils/jsnsr04t_distance_sensor.hpp>
#include <toml.hpp>
#include <ipc_common/net_broadcast.hpp>
#include <ipc_common/bluetooth_broadcast.hpp>

bool is_interrupted = false;

void signal_handler(int) {
    is_interrupted = true;
}

namespace carpi {
    int main(int argc, char *argv[]) {
        utils::Logger log{"Main"};

        auto cfg = toml::parse("resources/config.toml");
        auto sensor_cfg = toml::find(cfg, "jsnsr04t");
        auto bcst_conf = toml::find(sensor_cfg, "broadcast");

        const auto trig_pin = toml::find<uint32_t>(sensor_cfg, "trigger");
        const auto echo_pin = toml::find<uint32_t>(sensor_cfg, "echo");

        const auto udp_conf = toml::find(bcst_conf, "udp");
        const auto btconf = toml::find(bcst_conf, "bluetooth");

        std::shared_ptr<ipc::NetBroadcast> udp_bcast{};
        std::shared_ptr<ipc::BluetoothBroadcast> bt_bcast{};

        if (toml::find_or<bool>(udp_conf, "enabled", false)) {
            udp_bcast = std::make_shared<ipc::NetBroadcast>(toml::find<std::string>(udp_conf, "address"), toml::find<uint16_t>(udp_conf, "port"));
        }

        if (toml::find_or<bool>(btconf, "enabled", false)) {
            bt_bcast = std::make_shared<ipc::BluetoothBroadcast>(
                    toml::find<std::string>(btconf, "mode"),
                    toml::find_or<std::string>(btconf, "target", ""),
                    toml::find<uint8_t>(btconf, "channel")
            );
        }

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        carpi::wiring::JSNSR04TDistanceSensor sensor{trig_pin, echo_pin};
        while (!is_interrupted) {
            ipc::IpcPackage package{ipc::Opcodes::MSG_JSNSR04T_UPDATE};
            package << sensor.current_distance().has_signal << sensor.current_distance().distance;
            if (udp_bcast) {
                udp_bcast->send_packet(package);
            }

            if (bt_bcast) {
                bt_bcast->send_packet(package);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }

        return 0;
    }
}

int main(int argc, char *argv[]) {
    return carpi::main(argc, argv);
}