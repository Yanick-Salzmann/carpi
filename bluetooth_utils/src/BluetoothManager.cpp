#include "bluetooth_utils/BluetoothManager.hpp"

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothManager);

    BluetoothManager::BluetoothManager() {
        _device_id = hci_get_route(nullptr);
        if (_device_id < 0) {
            log->error("Error finding bluetooth device: {} (errno={})", error_to_string(), errno);
            throw std::runtime_error("Error finding bluetooth device");
        }

        _socket = hci_open_dev(_device_id);

        if (_socket < 0) {
            log->error("Error opening bluetooth device: {} (errno={})", error_to_string(), errno);
            throw std::runtime_error("Error opening bluetooth device");
        }

        log->info("Opened bluetooth device");

        char name_buffer[248]{};
        if(hci_read_local_name(_socket, sizeof name_buffer, name_buffer, 0) < 0) {
            log->warn("Unable to read local bluetooth device name");
        } else {
            log->info("Local bluetooth name: {}", std::string{name_buffer});
        }
    }

    std::string BluetoothManager::error_to_string() {
        char error_buffer[1024]{};
        return strerror_r(errno, error_buffer, sizeof error_buffer);
    }

    void BluetoothManager::scan_devices() {

    }
}