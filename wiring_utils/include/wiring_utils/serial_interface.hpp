#ifndef CARPI_WIRING_UTILS_SERIAL_INTERFACE_HPP
#define CARPI_WIRING_UTILS_SERIAL_INTERFACE_HPP

#include <string>
#include <common_utils/log.hpp>
#include <chrono>

namespace carpi::wiring {
    class SerialInterface {
        LOGGER;

        int _file;

    public:
        explicit SerialInterface(const std::string& device, uint32_t baud_rate);

        void write(const std::string& data);

        std::string read(std::chrono::nanoseconds timeout_ns);

        template<typename Rep, typename Period>
        std::string read_duration(std::chrono::duration<Rep, Period> timeout) {
            return read(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout));
        }
    };
}

#endif //CARPI_WIRING_UTILS_SERIAL_INTERFACE_HPP
