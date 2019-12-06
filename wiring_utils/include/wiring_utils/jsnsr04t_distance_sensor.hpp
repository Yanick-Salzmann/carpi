#ifndef CARPI_WIRING_UTILS_JSNSR04T_DISTANCE_HPP
#define CARPI_WIRING_UTILS_JSNSR04T_DISTANCE_HPP

#include <thread>
#include <memory>
#include <cstdint>
#include <common_utils/log.hpp>
#include "gpio.hpp"

namespace carpi::wiring {
    class JSNSR04TDistanceSensor {
        LOGGER;

    public:
        struct Measurement {
            bool has_signal;
            float distance;
        };

    private:
        Measurement _cur_measurement{.has_signal = false};

        std::thread _listen_thread;
        std::thread _trigger_thread;

        std::unique_ptr<GpioPin> _trigger_pin;
        std::unique_ptr<GpioPin> _echo_pin;

        bool _is_running = true;

        void trigger_callback();
        void echo_callback();

    public:
        JSNSR04TDistanceSensor(uint32_t trigger_pin, uint32_t echo_pin);
        ~JSNSR04TDistanceSensor();

        JSNSR04TDistanceSensor(JSNSR04TDistanceSensor&) = delete;
        JSNSR04TDistanceSensor(JSNSR04TDistanceSensor&&) = delete;

        void operator = (JSNSR04TDistanceSensor&) = delete;
        void operator = (JSNSR04TDistanceSensor&&) = delete;

        Measurement current_distance() const {
            return _cur_measurement;
        }
    };
}

#endif //CARPI_WIRING_UTILS_JSNSR04T_DISTANCE_HPP
