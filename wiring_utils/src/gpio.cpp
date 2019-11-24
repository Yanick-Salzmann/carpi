#include "wiring_utils/gpio.hpp"
#include <wiringPi.h>
#include <wiring_utils/wiring_context.hpp>

namespace carpi::wiring {
    void GpioPin::mode(GpioMode mode) {
        pinMode(_pin_index, mode == GpioMode::OUTPUT_MODE ? OUTPUT : INPUT);
    }

    void GpioPin::high() {
        digitalWrite(_pin_index, HIGH);
    }

    void GpioPin::low() {
        digitalWrite(_pin_index, LOW);
    }

    Gpio::Gpio() {
        WiringContext ctx;
        wiringPiSetupPhys();
    }
}