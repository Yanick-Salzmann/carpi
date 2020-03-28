#include <unistd.h>
#include "wiring_utils/tm1637_display.hpp"

namespace carpi::wiring {

    tm1637_display::tm1637_display(uint8_t pin_clk, uint8_t pin_dio, uint32_t bit_delay) :
            _bit_delay{bit_delay},
            _pin_clk{pin_clk},
            _pin_dio{pin_dio} {
        Gpio gpio{};

        _pin_clk.input();
        _pin_dio.input();
        _pin_clk.low();
        _pin_dio.low();
    }

    void tm1637_display::delay_bit() {
        usleep(_bit_delay);
    }

    void tm1637_display::start_transmission() {
        _pin_dio.output();
        delay_bit();
    }

    void tm1637_display::stop_transmission() {
        _pin_dio.output();
        delay_bit();
        _pin_clk.input();
        delay_bit();
        _pin_dio.input();
        delay_bit();
    }

    bool tm1637_display::write_byte(uint8_t value) {
        auto data = value;

        for (auto i = 0u; i < 8; ++i) {
            _pin_clk.output();
            delay_bit();

            if (data & 1u) {
                _pin_dio.input();
            } else {
                _pin_dio.output();
            }

            delay_bit();
            _pin_clk.input();
            delay_bit();
            data >>= 1u;
        }

        _pin_clk.output();
        _pin_dio.input();
        delay_bit();

        _pin_clk.input();
        delay_bit();

        const auto ack = _pin_dio.read();
        if (!ack) {
            _pin_dio.output();
        }

        delay_bit();
        _pin_clk.output();
        delay_bit();

        return ack != 0;
    }

    tm1637_display &tm1637_display::write_segments(const std::vector<uint8_t> &segments, uint8_t offset) {
        if (offset + segments.size() > 4) {
            throw std::runtime_error{""};
        }

        start_transmission();
        write_byte(TM1637_I2C_COMM1);
        stop_transmission();

        start_transmission();
        write_byte(static_cast<uint8_t>(TM1637_I2C_COMM2 + offset));

        for (auto i = 0u; i < segments.size(); ++i) {
            write_byte(segments[i]);
        }

        stop_transmission();

        start_transmission();
        write_byte(static_cast<uint8_t>(TM1637_I2C_COMM3 + encode_brightness()));
        stop_transmission();

        return *this;
    }

    tm1637_display &tm1637_display::clear() {
        static std::vector<uint8_t> data{0, 0, 0, 0};
        return write_segments(data);
    }

    void tm1637_display::show_number(int32_t number, int16_t base) {
        auto is_negative = false;
        if(base < 0) {
            base = -base;
            is_negative = true;
            number = -number;
        }

        std::vector<uint8_t> digits(4);
        if(!number) {
            digits.assign(4, encode_digit(0));
        } else {
            auto index = 0;
            for(; index < 4 && number; ++index) {
                digits[3 - index] = encode_digit(static_cast<uint8_t>(number % base));
                number /= base;
            }

            if(is_negative) {
                if(index > 3) {
                    index = 3;
                }

                digits[3 - index] = minus_segment;
            }
        }

        write_segments(digits);
    }
}