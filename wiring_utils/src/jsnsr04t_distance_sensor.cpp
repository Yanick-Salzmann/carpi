#include <memory>
#include "wiring_utils/jsnsr04t_distance_sensor.hpp"

namespace carpi::wiring {
    LOGGER_IMPL(JSNSR04TDistanceSensor);

    JSNSR04TDistanceSensor::JSNSR04TDistanceSensor(uint32_t trigger_pin, uint32_t echo_pin) {
        Gpio gpio{};

        _echo_pin = std::make_unique<GpioPin>(GpioPin{gpio.open_pin(echo_pin)});
        _trigger_pin = std::make_unique<GpioPin>(GpioPin{gpio.open_pin(trigger_pin)});

        _echo_pin->mode(GpioMode::INPUT_MODE);
        _trigger_pin->mode(GpioMode::OUTPUT_MODE);

        _trigger_thread = std::thread{
                [this]() { trigger_callback(); }
        };

        _listen_thread = std::thread{
                [this]() { echo_callback(); }
        };
    }

    JSNSR04TDistanceSensor::~JSNSR04TDistanceSensor() {
        _is_running = false;
        if (_listen_thread.joinable()) {
            _listen_thread.join();
        }

        if (_trigger_thread.joinable()) {
            _trigger_thread.join();
        }
    }

    void JSNSR04TDistanceSensor::trigger_callback() {
        while(_is_running) {
            _trigger_pin->high();
            std::this_thread::sleep_for(std::chrono::microseconds{100});
            _trigger_pin->low();
            std::this_thread::sleep_for(std::chrono::microseconds{5000});
        }
    }

    void JSNSR04TDistanceSensor::echo_callback() {
        auto was_high = false;
        std::chrono::high_resolution_clock::time_point start;
        while (_is_running) {
            const auto cur = _echo_pin->state();
            if (cur && !was_high) {
                was_high = true;
                start = std::chrono::high_resolution_clock::now();
            } else if(!cur && was_high) {
                was_high = false;
                const auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                _cur_measurement.has_signal = true;
                _cur_measurement.distance = diff / 58.0f;
            }
        }
    }
}