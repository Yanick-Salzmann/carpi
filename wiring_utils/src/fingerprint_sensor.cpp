#include <wiringSerial.h>
#include <cstring>
#include <unistd.h>
#include <common_utils/error.hpp>
#include <common_utils/conversion.hpp>
#include "wiring_utils/fingerprint_sensor.hpp"
#include <chrono>
#include <thread>

namespace carpi::wiring {
    LOGGER_IMPL(FingerprintSensor);

    FingerprintSensor::FingerprintSensor(const std::string &device, std::size_t baud_rate) {
        _sensor = serialOpen(device.c_str(), static_cast<const int>(baud_rate));
        if (_sensor < 0) {
            log->error("Error opening fingerprint sensor on {}: {} (errno={})", device, utils::error_to_string(), errno);
            throw std::runtime_error{"Error opening device"};
        }
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

    uint8_t FingerprintSensor::generate_checksum(const std::vector<uint8_t> &data, std::size_t offset, std::size_t length) {
        uint8_t checksum = 0;
        for (std::size_t i = 0; i < length; ++i, ++offset) {
            checksum ^= data[offset];
        }

        return checksum;
    }

    std::vector<uint8_t> FingerprintSensor::read_packet() {
        while (serialDataAvail(_sensor) <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }

        std::vector<uint8_t> ret{};
        ret.push_back(static_cast<uint8_t &&>(serialGetchar(_sensor)));
        if (ret.back() != START_OF_DATA) {
            log->warn("Data is out of sync, FPC did not return start of data byte (0xF5) as first byte");
        }

        for (auto i = 0; i < 200; ++i) {
            while (serialDataAvail(_sensor) <= 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
            }

            ret.push_back(static_cast<uint8_t &&>(serialGetchar(_sensor)));
            if (ret.back() == END_OF_DATA) {
                break;
            }
        }

        log->info("Received {}", utils::bytes_to_hex(ret));

        return ret;
    }

    uint8_t FingerprintSensor::checked_command_response(uint8_t command, uint32_t &ret_code) {
        auto response = read_packet();
        if (!verify_packet(response, true, command)) {
            return 0;
        }

        response = std::vector<uint8_t>{response.begin() + 1, response.end() - 2};

        switch (command) {
            case CMD_ENROLL1:
            case CMD_ENROLL2:
            case CMD_ENROLL3: {
                if (response[3] == ACK_SUCCESS) {
                    return 1;
                } else if (response[3] == ACK_USER_EXISTS) {
                    return ACK_USER_EXISTS;
                } else if (response[3] == ACK_USER_OCCUPIED) {
                    return ACK_USER_OCCUPIED;
                }
                log->info("CMD_ENROLL1/2/3 response: {}", response[3]);
                return 0;
            }

            case CMD_IDENTIFY_USER:
            case CMD_DELETE_ALL:
            case CMD_DELETE: {
                return static_cast<uint8_t>(response[3] == ACK_SUCCESS ? 1 : 0);
            }

            case CMD_USER_COUNT: {
                if (response[3] != ACK_SUCCESS) {
                    return 0;
                }

                ret_code = response[2];
                return 1;
            }

            case CMD_GET_USER_ID: {
                if (response[3] != ACK_SUCCESS) {
                    return 0;
                }

                return 1;
            }

            case CMD_SEARCH_USER: {
                if (response[3] >= 1 && response[3] <= 3) {
                    ret_code = response[1];
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
        write_packet(enroll_cmd, static_cast<uint8_t>(user_id & 0xFF), static_cast<uint8_t>(user_id >> 8), 1);
        const auto enroll_result = checked_command_response(enroll_cmd);
        if (enroll_result != 1) {
            log->warn("Enrolling new user failed. Error: {}", enroll_result);
            return false;
        }

        return true;
    }

    bool FingerprintSensor::clear() {
        write_packet(CMD_DELETE_ALL);
        return checked_command_response(CMD_DELETE_ALL) == 1;
    }

    uint32_t FingerprintSensor::user_count() {
        write_packet(CMD_USER_COUNT);
        uint32_t num_users = 0;
        if (checked_command_response(CMD_USER_COUNT, num_users) != 1) {
            return 0;
        }

        return num_users;
    }

    int32_t FingerprintSensor::find_user() {
        write_packet(CMD_SEARCH_USER);
        uint32_t user_match = 0;
        if (!checked_command_response(CMD_SEARCH_USER, user_match)) {
            return -1;
        }

        return static_cast<int32_t>(user_match);
    }

    bool FingerprintSensor::read_registered_users(std::vector<uint32_t> &user_ids) {
        auto packet = read_packet();
        if (!verify_packet(packet)) {
            return false;
        }

        packet = std::vector<uint8_t>{packet.begin() + 1, packet.end() - 2};
        const auto num_users = packet[1];
        for (auto i = 0; i < num_users; ++i) {
            user_ids.push_back(packet[3 + i]);
        }

        return true;
    }

    std::vector<uint32_t> FingerprintSensor::user_list() {
        write_packet(CMD_GET_USER_ID);
        checked_command_response(CMD_GET_USER_ID);

        std::vector<uint32_t> users{};
        if (!read_registered_users(users)) {
            return {};
        }

        return users;
    }

    bool FingerprintSensor::verify_packet(const std::vector<uint8_t> &response, bool validate_command, uint8_t command) {
        if (response.empty()) {
            return false;
        }

        if (response[0] != START_OF_DATA) {
            log->warn("FPC sensor data does not start with 0xF5");
            return false;
        }

        if (response[response.size() - 2] != generate_checksum(response, 1, response.size() - 3)) {
            log->warn("Checksum of FPC packet does not match");
            return false;
        }

        if (response[response.size() - 1] != END_OF_DATA) {
            log->warn("Partial response received, results might be wrong");
        }

        if (response[1] != command) {
            log->warn("Expected opcode {} from FPC sensor, but received {}", command, response[1]);
            return false;
        }

        return true;
    }

    bool FingerprintSensor::match_user(uint32_t user_id) {
        write_packet(CMD_IDENTIFY_USER, static_cast<uint8_t>(user_id >> 8u), static_cast<uint8_t>(user_id & 0xFFu));
        return checked_command_response(CMD_IDENTIFY_USER) != 0;
    }
}