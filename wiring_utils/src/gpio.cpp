#include "wiring_utils/gpio.hpp"
#include <wiringPi.h>
#include <wiring_utils/wiring_context.hpp>

namespace carpi::wiring {
    GpioPin& GpioPin::mode(GpioMode mode) {
        pinMode(_pin_index, mode == GpioMode::OUTPUT_MODE ? OUTPUT : INPUT);
        return *this;
    }

    GpioPin& GpioPin::high() {
        digitalWrite(_pin_index, HIGH);
        return *this;
    }

    GpioPin& GpioPin::low() {
        digitalWrite(_pin_index, LOW);
        return *this;
    }

    bool GpioPin::state() const {
        return digitalRead(_pin_index) == HIGH;
    }

    uint8_t GpioPin::read() {
        return digitalRead(_pin_index);
    }

    Gpio::Gpio() {
        WiringContext ctx;
        wiringPiSetupPhys();
    }
}