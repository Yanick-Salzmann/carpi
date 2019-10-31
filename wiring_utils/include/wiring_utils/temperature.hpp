#ifndef CARPI_WIRING_UTILS_TEMPERATURE_HPP
#define CARPI_WIRING_UTILS_TEMPERATURE_HPP

#include <cstdint>

namespace carpi::wiring {
    struct DHT11Result {
        float temperature;
        float humidity;
    };

    class DHT11TemperatureSensor {
        uint32_t _gpio_pin = 0;
        uint32_t _timing_count = 0;

    public:
        explicit DHT11TemperatureSensor(uint32_t gpio_pin, uint32_t timing_count = 85);

        bool read_values(DHT11Result& result);
    };
}

#endif //CARPI_WIRING_UTILS_TEMPERATURE_HPP
