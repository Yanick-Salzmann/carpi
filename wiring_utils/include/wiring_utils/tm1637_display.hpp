#ifndef CARPI_WIRING_UTILS_TM1637_DISPLAY_HPP
#define CARPI_WIRING_UTILS_TM1637_DISPLAY_HPP

#include <cstdint>
#include <array>
#include <vector>
#include "gpio.hpp"

namespace carpi::wiring {
    class tm1637_display {
    public:
        static constexpr uint32_t default_bit_delay = 100;

    private:
        static constexpr uint32_t TM1637_I2C_COMM1 = 0x40;
        static constexpr uint32_t TM1637_I2C_COMM2 = 0xC0;
        static constexpr uint32_t TM1637_I2C_COMM3 = 0x80;
        static constexpr uint8_t minus_segment = 0b01000000;

        uint32_t _bit_delay;
        uint8_t _brightness = 7;
        bool _is_turned_on = true;

        GpioPin _pin_clk;
        GpioPin _pin_dio;

        void delay_bit();

        void start_transmission();
        void stop_transmission();

        bool write_byte(uint8_t value);

        uint8_t encode_brightness() {
            return (uint8_t)((_brightness & 7u) | (_is_turned_on ? 0x08u : 0x00u));
        }

        void show_number(int32_t number, int16_t base);

        static uint8_t encode_digit(uint8_t value) {
            static uint8_t digit_to_segment_map[] = {
                    0b00111111,    // 0
                    0b00000110,    // 1
                    0b01011011,    // 2
                    0b01001111,    // 3
                    0b01100110,    // 4
                    0b01101101,    // 5
                    0b01111101,    // 6
                    0b00000111,    // 7
                    0b01111111,    // 8
                    0b01101111,    // 9
                    0b01110111,    // A
                    0b01111100,    // b
                    0b00111001,    // C
                    0b01011110,    // d
                    0b01111001,    // E
                    0b01110001     // F
            };

            return digit_to_segment_map[value];
        }



    public:
        tm1637_display(uint8_t pin_clk, uint8_t pin_dio, uint32_t bit_delay = default_bit_delay);

        tm1637_display& brightness(uint8_t brightness) {
            _brightness = (uint8_t) (brightness & 0x07u);
            return *this;
        }

        tm1637_display& turn_on() {
            _is_turned_on = true;
            return *this;
        }

        tm1637_display& turn_off() {
            _is_turned_on = false;
            return *this;
        }

        tm1637_display& write_segments(const std::vector<uint8_t>& segments, uint8_t offset = 0);
        tm1637_display& clear();

        tm1637_display& show_number(int16_t number) {
            show_number(number, static_cast<int16_t>(number < 0 ? -10 : 10));
            return *this;
        }

        tm1637_display& show_hex_number(uint16_t number) {
            show_number(number, 16);
            return *this;
        }
    };
}

#endif //CARPI_WIRING_UTILS_TM1637_DISPLAY_HPP
