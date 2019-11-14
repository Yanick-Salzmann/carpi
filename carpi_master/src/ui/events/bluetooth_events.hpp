#ifndef CARPI_CARPI_BLUETOOTH_EVENTS_HPP
#define CARPI_CARPI_BLUETOOTH_EVENTS_HPP

#include <vector>
#include <string>

#include <common_utils/singleton.hpp>
#include <nlohmann/json.hpp>
#include <bluetooth_utils/BluetoothServer.hpp>

namespace carpi::ui::events {
    struct BluetoothDeviceInfo {
        std::string name;
        std::string address;
    };

    struct BluetoothResponse {
        std::vector<BluetoothDeviceInfo> devices;

        [[nodiscard]] nlohmann::json to_json() const;
    };

    class BluetoothEvents : public utils::Singleton<BluetoothEvents> {
        BluetoothResponse fetch_devices();

    public:
        void init_events();
    };
}

#define sBluetoothEvents (carpi::ui::events::BluetoothEvents::instance())

#endif //CARPI_CARPI_BLUETOOTH_EVENTS_HPP
