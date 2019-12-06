#include "bluetooth_utils/bluetooth_manager.hpp"
#include "common_utils/error.hpp"

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothManager);

    BluetoothManager::BluetoothManager() {
        _device_id = hci_get_route(nullptr);
        if (_device_id < 0) {
            log->error("Error finding bluetooth device: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error finding bluetooth device");
        }

        _socket = hci_open_dev(_device_id);

        if (_socket < 0) {
            log->error("Error opening bluetooth device: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error opening bluetooth device");
        }

        log->info("Opened bluetooth device");

        char name_buffer[248]{};
        if (hci_read_local_name(_socket, sizeof name_buffer, name_buffer, 0) < 0) {
            log->warn("Unable to read local bluetooth device name");
        } else {
            log->info("Local bluetooth name: {}", std::string{name_buffer});
        }
    }

    std::set<BluetoothDevice> BluetoothManager::scan_devices(uint32_t search_ticks) {
        std::array<inquiry_info, 255> device_inquiry{};
        auto ii = device_inquiry.data();
        const auto num_devices = hci_inquiry(_device_id, search_ticks, 255, nullptr, &ii, IREQ_CACHE_FLUSH);
        if (num_devices < 0) {
            log->warn("Error scanning for devices: {} (errno={})", utils::error_to_string(), errno);
            return {};
        }

        log->debug("Scanned for devices, found {}", num_devices);

        std::set<BluetoothDevice> ret_set{};

        for (auto i = 0; i < num_devices; ++i) {
            ret_set.emplace(_socket, device_inquiry[i].bdaddr);
        }

        return ret_set;
    }

    BluetoothDevice BluetoothManager::open_device(const std::string &address) {
        bdaddr_t addr{};
        str2ba(address.c_str(), &addr);

        return BluetoothDevice{_socket, addr};
    }
}