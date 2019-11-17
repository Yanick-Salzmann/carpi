#include "bluetooth_events.hpp"
#include "ui/event_manager.hpp"
#include <bluetooth_utils/BluetoothManager.hpp>
#include <io/obd/ObdConnectionManager.hpp>

namespace carpi::ui::events {
    LOGGER_IMPL(BluetoothEvents);
    LOGGER_IMPL(BluetoothDeviceInfo);

    using nlohmann::json;

    nlohmann::json BluetoothResponse::to_json() const {
        json ret{};
        for (const auto &dev : devices) {
            ret.emplace_back(json{
                    {"name",    dev.name},
                    {"address", dev.address}
            });
        }

        return ret;
    }

    BluetoothResponse BluetoothEvents::fetch_devices() {
        const auto devices = sBluetoothMgr->scan_devices(4);
        std::vector<BluetoothDeviceInfo> devs{};
        for (const auto &dev : devices) {
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
        sUiEventMgr->register_event_handler<NoOp, BluetoothResponse>("bluetooth_search", [this](const NoOp &arg) {
            return fetch_devices();
        });

        sUiEventMgr->register_event_handler<BluetoothDeviceInfo, ObdConnectResponse>("obd_connect", [this](const BluetoothDeviceInfo &arg) {
            return create_obd_connection(arg);
        });

        sUiEventMgr->register_event_handler<NoOp, ObdStatusResponse>("obd_status", [this](const NoOp& arg) {
            return obd_status();
        });
    }

    ObdConnectResponse BluetoothEvents::create_obd_connection(const BluetoothDeviceInfo &device) {
        return {
                .is_success = sObdConnMgr->connect_to_device(device.address)
        };
    }

    ObdStatusResponse BluetoothEvents::obd_status() {
        return {
            .rpm = sObdConnMgr->fetch_rpm(),
            .speed = sObdConnMgr->fetch_speed()
        };
    }

    BluetoothDeviceInfo BluetoothDeviceInfo::from_json(const nlohmann::json &request) {
        auto itr = request.find("address");
        if (itr == request.end()) {
            log->error("Error parsing BluetoothDeviceInfo from JSON '{}', no 'address' element found", request.dump());
            throw std::runtime_error{"Error parsing BluetoothDeviceInfo"};
        }

        std::string addr = itr.value();
        std::string name{};

        itr = request.find("name");
        if (itr != request.end()) {
            name = itr.value();
        }

        return BluetoothDeviceInfo{
                .name = name,
                .address = addr
        };
    }

    nlohmann::json ObdConnectResponse::to_json() const {
        return json{
                {"success", is_success}
        };
    }

    nlohmann::json ObdStatusResponse::to_json() const {
        return json {
                {"speed", speed},
                {"rpm", rpm}
        };
    }
}