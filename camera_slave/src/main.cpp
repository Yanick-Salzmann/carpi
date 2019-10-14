#include <iostream>
#include <bluetooth_utils/BluetoothManager.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        utils::log::Logger log{"main"};
        bluetooth::BluetoothManager mgr{};
        for(auto device : mgr.scan_devices(std::chrono::seconds{10})) {
            log->info("Device: {} @ {}", device.device_name(), device.address_string());
            device.connect(0x1001, 0x1234);
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}