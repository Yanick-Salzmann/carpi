#include <wiring_utils/wiring_context.hpp>
#include "wiring_utils/temperature.hpp"

#include <wiringPi.h>

namespace carpi::wiring {
    DHT11TemperatureSensor::DHT11TemperatureSensor(uint32_t gpio_pin, uint32_t timing_count) : _gpio_pin{gpio_pin}, _timing_count{timing_count} {
        static WiringContext ctx{};
    }

    bool DHT11TemperatureSensor::read_values(DHT11Result &result) {
        int dht11_dat[5] = {0, 0, 0, 0, 0};

        uint8_t laststate = HIGH;
        uint8_t j = 0;
        float f;

        dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

        pinMode(_gpio_pin, OUTPUT);
        digitalWrite(_gpio_pin, LOW);
        delay(18);
        digitalWrite(_gpio_pin, HIGH);
        delayMicroseconds(40);
        pinMode(_gpio_pin, INPUT);

        for (uint32_t i = 0; i < _timing_count; i++) {
            uint8_t counter = 0;
            while (digitalRead(_gpio_pin) == laststate) {
                counter++;
                delayMicroseconds(1);
                if (counter == 255) {
                    break;
                }
            }
            laststate = digitalRead(_gpio_pin);

            if (counter == 255)
                break;

            if ((i >= 4) && (i % 2 == 0)) {
                dht11_dat[j / 8u] <<= 1u;
                if (counter > 16)
                    dht11_dat[j / 8] |= 1;
                j++;
            }
        }

        if ((j >= 40) &&
            (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF))) {
        }

        return {};
    }
}