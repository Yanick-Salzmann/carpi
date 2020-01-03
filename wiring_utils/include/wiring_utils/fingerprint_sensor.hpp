#ifndef CARPI_WIRING_UTILS_FINGERPRINT_SENSOR_HPP
#define CARPI_WIRING_UTILS_FINGERPRINT_SENSOR_HPP

#include <string>
#include <vector>

#include <common_utils/log.hpp>

namespace carpi::wiring {
    class FingerprintSensor {
        enum Command : uint8_t {
            CMD_ENROLL1 = 0x01,
            CMD_ENROLL2 = 0x02,
            CMD_ENROLL3 = 0x03,
            CMD_DELETE = 0x04,
            CMD_DELETE_ALL = 0x05,
            CMD_USER_COUNT = 0x06,
            CMD_IDENTIFY_USER = 0x07,
            CMD_SEARCH_USER = 0x08,
            CMD_GET_USER_ID = 0x09
        };

        enum Acknowledgment : uint8_t {
            ACK_SUCCESS = 0,
            ACK_FAIL = 1,
            ACK_DB_FULL = 4,
            ACK_NOT_FOUND = 5,
            ACK_USER_OCCUPIED = 6,
            ACK_USER_EXISTS = 7,
            ACK_TIMEOUT = 8
        };

        static const uint8_t START_OF_DATA = 0xF5;
        static const uint8_t END_OF_DATA = 0xF5;

        LOGGER;

        int32_t _sensor;

        static uint8_t generate_checksum(const std::vector<uint8_t>& data) { generate_checksum(data, 0, data.size()); }
        static uint8_t generate_checksum(const std::vector<uint8_t>& data, std::size_t offset, std::size_t length);

        void write_packet(Command command, uint8_t b1) {
            write_packet(command, b1, 0, 0, 0);
        }

        void write_packet(Command command, uint8_t b1, uint8_t b2) {
            write_packet(command, b1, b2, 0, 0);
        }

        void write_packet(Command command, uint8_t b1, uint8_t b2, uint8_t b3) {
            write_packet(command, b1, b2, b3, 0);
        }
        void write_packet(Command command, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
            write_packet(std::vector<uint8_t>{command, b1, b2, b3, b4});
        }

        void write_packet(Command command) {
            write_packet(command, 0, 0, 0, 0);
        }

        void write_packet(const std::vector<uint8_t>& data);
        std::vector<uint8_t> read_packet();

        uint8_t checked_command_response(uint8_t command) {
            uint8_t ignored{};
            return checked_command_response(command, ignored);
        }

        uint8_t checked_command_response(uint8_t command, uint8_t& response);

        bool enroll_step(Command enroll_cmd, uint16_t user_id);
    public:
        explicit FingerprintSensor(const std::string& device, std::size_t baud_rate);

        bool enroll(uint16_t user_id);
        bool clear();

        uint8_t user_count();
    };
}

#endif //CARPI_WIRING_UTILS_FINGERPRINT_SENSOR_HPP
