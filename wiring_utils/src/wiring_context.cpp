#include "wiring_utils/wiring_context.hpp"

#include <atomic>
#include <mutex>

#include <wiringPi.h>

namespace carpi::wiring {
    static std::once_flag _load_flag{};

    WiringContext::WiringContext() {
        std::call_once(_load_flag, []() {
            if(wiringPiSetup() == -1) {
                throw std::runtime_error{"Error initializing wiringPi"};
            }
        });
    }
}