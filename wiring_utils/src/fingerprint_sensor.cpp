#include <wiringSerial.h>
#include <cstring>
#include <unistd.h>
#include <common_utils/error.hpp>
#include "wiring_utils/fingerprint_sensor.hpp"

namespace carpi::wiring {
    LOGGER_IMPL(FingerprintSensor);

    FingerprintSensor::FingerprintSensor(const std::string &device, std::size_t baud_rate) {
        _sensor = serialOpen(device.c_str(), static_cast<const int>(baud_rate));
    }

    void FingerprintSensor::write_packet(const std::vector<uint8_t> &data) {
        std::vector<uint8_t> full_data(data.size() + 3);
        full_data[0] = START_OF_DATA;

        memcpy(&full_data[1], data.data(), data.size());

        full_data[full_data.size() - 2] = generate_checksum(data);
        full_data[full_data.size() - 1] = END_OF_DATA;

        if (write(_sensor, full_data.data(), full_data.size()) != full_data.size()) {
            log->error("Error writing packet to fingerprint sensor: {} (errno={})", errno, utils::error_to_string());
            throw std::runtime_error{"Error sending data"};
        }
    }

    uint8_t FingerprintSensor::generate_checksum(const std::vector<uint8_t> &data) {
        uint8_t checksum = 0;
        for (const auto &elem : data) {
            checksum ^= elem;
        }

        return checksum;
    }

    std::vector<uint8_t> FingerprintSensor::read_packet() {
        while (serialDataAvail(_sensor) <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }

        std::vector<uint8_t> ret{};
        for (auto i = 0; i < 200; ++i) {
            ret.push_back(static_cast<uint8_t &&>(serialGetchar(_sensor)));
            if (!serialDataAvail(_sensor)) {
                std::this_thread::sleep_for(std::chrono::milliseconds{50});
                if (!serialDataAvail(_sensor)) {
                    break;
                }
            }
        }

        log->info("Read {} bytes", ret.size());

        return ret;
    }

    uint8_t FingerprintSensor::checked_command_response(uint8_t command, uint8_t &ret_code) {
        auto response = read_packet();
        if (response.empty()) {
            return 0;
        }

        switch (command) {
            case CMD_ENROLL1:
            case CMD_ENROLL2:
            case CMD_ENROLL3: {
                if (response[4] == ACK_SUCCESS) {
                    return 1;
                } else if (response[4] == ACK_USER_EXISTS) {
                    return ACK_USER_EXISTS;
                } else if (response[4] == ACK_USER_OCCUPIED) {
                    return ACK_USER_OCCUPIED;
                }
                return 0;
            }

            case CMD_IDENTIFY_USER:
            case CMD_DELETE_ALL:
            case CMD_DELETE: {
                return static_cast<uint8_t>(response[4] == ACK_SUCCESS ? 1 : 0);
            }

            case CMD_USER_COUNT:
            case CMD_GET_USER_ID: {
                if (response[4] != ACK_SUCCESS) {
                    return 0;
                }

                ret_code = response[3];
                return 1;
            }

            case CMD_SEARCH_USER: {
                if (response[4] >= 1 && response[4] <= 3) {
                    ret_code = response[3];
                    return 1;
                }

                return 0;
            }

            default: {
                return 0;
            }
        }
    }

    bool FingerprintSensor::enroll(uint16_t user_id) {
        return enroll_step(CMD_ENROLL1, user_id) &&
               enroll_step(CMD_ENROLL2, user_id) &&
               enroll_step(CMD_ENROLL3, user_id);
    }

    bool FingerprintSensor::enroll_step(FingerprintSensor::Command enroll_cmd, uint16_t user_id) {
        write_packet(enroll_cmd, static_cast<uint8_t>(user_id >> 8u), static_cast<uint8_t>(user_id & 0xFFu), 1);
        const auto enroll_result = checked_command_response(enroll_cmd);
        if(enroll_result != 1) {
            log->warn("Enrolling new user failed. Error: {}", enroll_result);
            return false;
        }

        return true;
    }

    bool FingerprintSensor::clear() {
        write_packet(CMD_DELETE_ALL);
        return checked_command_response(CMD_DELETE_ALL) == 1;
    }
}