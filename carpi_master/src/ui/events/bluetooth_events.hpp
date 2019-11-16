#ifndef CARPI_CARPI_BLUETOOTH_EVENTS_HPP
#define CARPI_CARPI_BLUETOOTH_EVENTS_HPP

#include <vector>
#include <string>

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>
#include <nlohmann/json.hpp>
#include <bluetooth_utils/BluetoothServer.hpp>

namespace carpi::ui::events {
    struct BluetoothDeviceInfo {
        LOGGER;

        std::string name;
        std::string address;

        static BluetoothDeviceInfo from_json(const nlohmann::json& request);
    };

    struct BluetoothResponse {
        std::vector<BluetoothDeviceInfo> devices;

        [[nodiscard]] nlohmann::json to_json() const;
    };

    struct ObdConnectResponse {
        bool is_success;

        [[nodiscard]] nlohmann::json to_json() const;
    };

    class BluetoothEvents : public utils::Singleton<BluetoothEvents> {
        LOGGER;

        static BluetoothResponse fetch_devices();
        static ObdConnectResponse create_obd_connection(const BluetoothDeviceInfo& device);

    public:
        void init_events();
    };
}

#define sBluetoothEvents (carpi::ui::events::BluetoothEvents::instance())

#endif //CARPI_CARPI_BLUETOOTH_EVENTS_HPP
