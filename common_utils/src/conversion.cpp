#include "common_utils/conversion.hpp"

namespace carpi::utils {
    uint8_t hex_char_to_byte(char c) {
        if(c >= 'A' && c <= 'F') {
            return static_cast<uint8_t>(c - 'A') + 10;
        } else if(c >= 'a' && c <= 'f') {
            return static_cast<uint8_t>(c - 'a') + 10;
        } else if(c >= '0' && c <= '9') {
            return static_cast<uint8_t>(c - '0');
        } else {
            return -1;
        }
    }

    std::vector<uint8_t> hex2bytes(const std::string& hex) {
        std::vector<uint8_t> bytes{};
        bytes.reserve(hex.size() / 2);

        for(auto i = std::size_t{0}; i < hex.size() / 2; ++i) {
            auto c0 = hex[i * 2];
            auto c1 = hex[i * 2 + 1];
            bytes.emplace_back(hex_char_to_byte(c1) | (hex_char_to_byte(c0) << 4u));
        }

        return bytes;
    }
}