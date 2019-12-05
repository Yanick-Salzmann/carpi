#include <iostream>

#include <common_utils/log.hpp>
#include <wiring_utils/serial_interface.hpp>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        log->error("Usage: {} <serial_device> <baud_rate>", argv[0]);
        return 1;
    }

    carpi::wiring::SerialInterface iface{argv[1], (uint32_t) std::stoul(argv[2])};
    bool run = true;

    std::thread t{
        [&iface, &run]() {
            while(run) {
                iface.write("F");
                log->info("Distance: {}", iface.read(std::chrono::milliseconds{500}));
            }
        }
    };

    std::string str;
    while(std::getline(std::cin, str)) {
        if(str.find("quit") != std::string::npos) {
            break;
        }
    }

    run = false;
    t.join();
}