#ifndef CARPI_WIRING_UTILS_GPIO_HPP
#define CARPI_WIRING_UTILS_GPIO_HPP

#include <cstdint>

namespace carpi::wiring {
    enum class GpioMode {
        OUTPUT_MODE,
        INPUT_MODE
    };

    class GpioPin {
        GpioMode  _mode = GpioMode::INPUT_MODE;
        uint32_t _pin_index = 0;

    public:
        explicit GpioPin(uint32_t index) : _pin_index{index} {

        }

        GpioPin& mode(GpioMode mode);

        GpioPin& input() {
            mode(GpioMode::INPUT_MODE);
            return *this;
        }

        GpioPin& output() {
            mode(GpioMode::OUTPUT_MODE);
            return *this;
        }

        [[nodiscard]] auto mode() const -> GpioMode {
            return _mode;
        }

        GpioPin& high();
        GpioPin& low();

        [[nodiscard]] bool state() const;

        uint8_t read();
    };

    class Gpio {
    public:
        Gpio();

        GpioPin open_pin(uint32_t index) {
            return GpioPin{index};
        }
    };
}

#endif //CARPI_WIRING_UTILS_GPIO_HPP
