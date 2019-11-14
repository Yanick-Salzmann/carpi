#include "bluetooth_events.hpp"
#include "ui/event_manager.hpp"
#include <bluetooth_utils/BluetoothManager.hpp>

namespace carpi::ui::events {
    using nlohmann::json;

    nlohmann::json BluetoothResponse::to_json() const {
        json ret{};
        for(const auto& dev : devices) {
            ret.emplace_back(json{
                    { "name", dev.name },
                    { "address", dev.address }
            });
        }

        return ret;
    }

    BluetoothResponse BluetoothEvents::fetch_devices() {
        const auto devices = sBluetoothMgr->scan_devices(4);
        std::vector<BluetoothDeviceInfo> devs{};
        for(const auto& dev : devices) {
            devs.emplace_back(BluetoothDeviceInfo{
                .name = dev.device_name(),
                .address = dev.address_string()
            });
        }

        return BluetoothResponse{
            .devices = devs
        };
    }

    void BluetoothEvents::init_events() {
        sUiEventMgr->register_event_handler<NoOp, BluetoothResponse>("bluetooth_search", [this](const NoOp& arg) {
            return fetch_devices();
        });
    }
}