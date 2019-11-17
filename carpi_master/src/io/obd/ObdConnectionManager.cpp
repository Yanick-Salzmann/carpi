#include "ObdConnectionManager.hpp"
#include <bluetooth_utils/BluetoothManager.hpp>
#include <obd/ObdCommandList.hpp>

namespace carpi::io::obd {
    bool ObdConnectionManager::connect_to_device(const std::string &address) {
        try {
            const auto device = sBluetoothMgr->open_device(address);
            _connection = std::make_shared<carpi::obd::ObdInterface>(device.connect(1));
        } catch (std::runtime_error &e) {
            return false;
        }

        return true;
    }

    uint32_t ObdConnectionManager::fetch_speed() {
        utils::Any speed{};
        if (!_connection->send_command(carpi::obd::COMMAND_LIST.find("SPEED")->second, speed)) {
            return 0;
        }

        return utils::any_cast<uint32_t>(speed);
    }

    float ObdConnectionManager::fetch_rpm() {
        utils::Any rpm{};
        if(!_connection->send_command(carpi::obd::COMMAND_LIST.find("RPM")->second, rpm)) {
            return 0;
        }

        return utils::any_cast<float>(rpm);
    }
}
