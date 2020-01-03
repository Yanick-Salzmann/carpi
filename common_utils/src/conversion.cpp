#include "common_utils/conversion.hpp"

#include <sstream>
#include <iomanip>

#include "helper/basen.hpp"

namespace carpi::utils {
    uint8_t hex_char_to_byte(char c) {
        if (c >= 'A' && c <= 'F') {
            return static_cast<uint8_t>((c - 'A') + 10);
        } else if (c >= 'a' && c <= 'f') {
            return static_cast<uint8_t>((c - 'a') + 10);
        } else if (c >= '0' && c <= '9') {
            return static_cast<uint8_t>(c - '0');
        } else {
            return static_cast<uint8_t>(-1);
        }
    }

    std::vector<uint8_t> hex2bytes(const std::string &hex) {
        std::vector<uint8_t> bytes{};
        bytes.reserve(hex.size() / 2);

        for (auto i = std::size_t{0}; i < hex.size() / 2; ++i) {
            auto c0 = hex[i * 2];
            auto c1 = hex[i * 2 + 1];
            bytes.emplace_back(hex_char_to_byte(c1) | (hex_char_to_byte(c0) << 4u));
        }

        return bytes;
    }

    std::string bytes_to_hex(const void *binary, std::size_t length) {
        std::stringstream stream{};
        const auto ptr = (const uint8_t*) binary;
        for (auto i = std::size_t{0}; i < length; ++i) {
            stream << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (uint32_t) ptr[i];
        }

        return stream.str();
    }

    std::vector<uint8_t> utf8_to_bytes(const std::string &str) {
        return std::vector<uint8_t>{str.begin(), str.end()};
    }

    std::string base64_encode(const std::vector<uint8_t> &bytes) {
        std::string encoded{};
        bn::encode_b64(bytes.begin(), bytes.end(), std::back_inserter(encoded));
        return encoded;
    }

    std::string bytes_to_utf8(const std::vector<uint8_t> &bytes) {
        return std::string{bytes.begin(), bytes.end()};
    }
}