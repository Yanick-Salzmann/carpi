#include <iostream>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include <interface/mmal/mmal.h>

namespace carpi {
    int main(int argc, char* argv[]) {
        char hostname[HOST_NAME_MAX];
        char username[LOGIN_NAME_MAX];

        gethostname(hostname, HOST_NAME_MAX);
        getlogin_r(username, LOGIN_NAME_MAX);

        utils::Logger log{"main"};
        log->info("MMAL v{}.{}", MMAL_VERSION_MAJOR, MMAL_VERSION_MINOR);

        bluetooth::BluetoothManager mgr{};
        for(auto device : mgr.scan_devices(std::chrono::seconds{10})) {
            log->info("Device: {} @ {}", device.device_name(), device.address_string());
            auto connection = device.connect(1);
            *connection << fmt::format("Hello from {}@{}", std::string{username}, std::string{hostname});
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}