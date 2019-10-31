#include <wiring_utils/wiring_context.hpp>
#include "wiring_utils/temperature.hpp"

#include <wiringPi.h>

namespace carpi::wiring {
    DHT11TemperatureSensor::DHT11TemperatureSensor(uint32_t gpio_pin, uint32_t timing_count) : _gpio_pin{gpio_pin}, _timing_count{timing_count} {
        static WiringContext ctx{};
    }

    bool DHT11TemperatureSensor::read_values(DHT11Result &result) {
        uint32_t dht11_dat[5] = {0, 0, 0, 0, 0};

        uint8_t last_state = HIGH;
        uint8_t j = 0;

        pinMode(_gpio_pin, OUTPUT);
        digitalWrite(_gpio_pin, LOW);
        delay(18);
        digitalWrite(_gpio_pin, HIGH);
        delayMicroseconds(40);
        pinMode(_gpio_pin, INPUT);

        for (uint32_t i = 0; i < _timing_count; i++) {
            uint8_t counter = 0;
            while (digitalRead(_gpio_pin) == last_state) {
                counter++;
                delayMicroseconds(1);
                if (counter == 255) {
                    break;
                }
            }

            last_state = digitalRead(_gpio_pin);

            if (counter == 255)
                break;

            if ((i >= 4) && (i % 2 == 0)) {
                dht11_dat[j / 8u] <<= 1u;
                if (counter > 16)
                    dht11_dat[j / 8] |= 1u;
                j++;
            }
        }

        if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFFu))) {
            const auto hum_raw = (dht11_dat[0] << 8u) | dht11_dat[1];
            const auto temp_raw = (dht11_dat[2] << 8u) | dht11_dat[3];
            const auto hum = hum_raw / 256.0f;
            const auto temp = temp_raw / 256.0f;

            result.humidity = hum;
            result.temperature = temp;
            return true;
        }

        return false;
    }
}