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

        void mode(GpioMode mode);

        [[nodiscard]] auto mode() const -> GpioMode {
            return _mode;
        }

        void high();
        void low();
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
