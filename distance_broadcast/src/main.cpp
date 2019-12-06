#include <iostream>

#include <common_utils/log.hpp>
#include <wiring_utils/serial_interface.hpp>
#include <wiring_utils/gpio.hpp>
#include <wiring_utils/jsnsr04t_distance_sensor.hpp>

int main(int argc, char *argv[]) {
    carpi::utils::Logger log{"Main"};

//    if(argc < 3) {
//        log->error("Usage: {} <serial_device> <baud_rate>", argv[0]);
//        return 1;
//    }
//
//    carpi::wiring::SerialInterface iface{argv[1], (uint32_t) std::stoul(argv[2])};
//    bool run = true;
//
//    std::thread t{
//        [&iface, &run, &log]() {
//            while(run) {
//                iface.write("F");
//                log->info("Distance: {}", iface.read_duration(std::chrono::milliseconds{10}));
//            }
//        }
//    };
//
//    std::string str;
//    while(std::getline(std::cin, str)) {
//        if(str.find("quit") != std::string::npos) {
//            break;
//        }
//    }
//
//    run = false;
//    t.join();

    carpi::wiring::JSNSR04TDistanceSensor sensor{12, 11};
    for(auto i = 0; i < 100; ++i) {
        log->info("Distance: {}", sensor.current_distance().distance);
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}