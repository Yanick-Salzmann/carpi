#include <iostream>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        char hostname[HOST_NAME_MAX];
        char username[LOGIN_NAME_MAX];

        gethostname(hostname, HOST_NAME_MAX);
        getlogin_r(username, LOGIN_NAME_MAX);

        utils::log::Logger log{"main"};
        bluetooth::BluetoothManager mgr{};
        for(auto device : mgr.scan_devices(std::chrono::seconds{10})) {
            log->info("Device: {} @ {}", device.device_name(), device.address_string());
            auto connection = device.connect(0x1001);
            *connection << fmt::format("Hello from {}@{}", std::string{username}, std::string{hostname});
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}