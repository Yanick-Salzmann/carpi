#include <wiring_utils/serial_interface.hpp>
#include <wiringSerial.h>
#include <common_utils/error.hpp>

namespace carpi::wiring {
    LOGGER_IMPL(SerialInterface);

    SerialInterface::SerialInterface(const std::string &device, uint32_t baud_rate) {
        _file = serialOpen(device.c_str(), baud_rate);
        if (_file < 0) {
            log->error("Error opening serial interface {}: {} (errno={})", device, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening serial interface"};
        }
    }

    void SerialInterface::write(const std::string &data) {
        serialPuts(_file, data.c_str());
    }

    std::string SerialInterface::read(std::chrono::nanoseconds timeout_ns) {
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(timeout_ns);
        timeout_ns -= secs;

        timespec sleep_val{
                .tv_sec = secs.count(),
                .tv_nsec = timeout_ns.count()
        };

        log->info("Timeout: secs {}, ns {}", sleep_val.tv_sec, sleep_val.tv_nsec);

        auto has_read = false;
        std::string ret_val{};
        int next_char;
        do {
            const auto num_available = serialDataAvail(_file);
            if (!num_available) {
                if (has_read) {
                    break;
                }

                const auto result = nanosleep(&sleep_val, nullptr);
                if (result != 0 && result != EINTR) {
                    log->error("Error sleeping: {} (errno={})", utils::error_to_string(errno), errno);
                    throw std::runtime_error{"Error reading from serial interface"};
                }
            }

            if (num_available < 0) {
                return ret_val;
            }

            has_read = true;

            next_char = serialGetchar(_file);
            if (next_char == '\n' || next_char == '\0') {
                break;
            }

            if (next_char != -1) {
                ret_val += (char) next_char;
            }
        } while (next_char != -1);

        return ret_val;
    }

}