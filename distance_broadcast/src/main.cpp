#include <iostream>

#include <common_utils/log.hpp>
#include <wiring_utils/serial_interface.hpp>
#include <wiring_utils/gpio.hpp>

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

    carpi::wiring::Gpio gpio{};
    auto trig = gpio.open_pin(12);
    auto echo = gpio.open_pin(11);
    trig.mode(carpi::wiring::GpioMode::OUTPUT_MODE);
    echo.mode(carpi::wiring::GpioMode::INPUT_MODE);

    auto run = true;

    std::thread t{
            [&echo, &run, log]() {
                auto was_high = false;
                std::chrono::high_resolution_clock::time_point start;
                while (run) {
                    const auto cur = echo.state();
                    if (cur && !was_high) {
                        was_high = true;
                        start = std::chrono::high_resolution_clock::now();
                    } else if(!cur && was_high) {
                        was_high = false;
                        const auto diff = std::chrono::high_resolution_clock::now() - start;
                        log->info("Time: {} us", std::chrono::duration_cast<std::chrono::microseconds>(diff).count());
                    }
                }
            }
    };

    for(auto i = 0; i < 10; ++i) {
        log->info("Trigger");
        trig.high();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        trig.low();
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }

    run = false;
    t.join();
}