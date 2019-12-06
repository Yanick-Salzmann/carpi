#include "wiring_utils/gpio.hpp"
#include <wiringPi.h>
#include <wiring_utils/wiring_context.hpp>

namespace carpi::wiring {
    void GpioPin::mode(GpioMode mode) {
        pinMode(_pin_index, mode == GpioMode::OUTPUT_MODE ? OUTPUT : INPUT);
    }

    void GpioPin::high() const {
        digitalWrite(_pin_index, HIGH);
    }

    void GpioPin::low() const {
        digitalWrite(_pin_index, LOW);
    }

    bool GpioPin::state() const {
        return digitalRead(_pin_index) == HIGH;
    }

    Gpio::Gpio() {
        WiringContext ctx;
        wiringPiSetupPhys();
    }
}