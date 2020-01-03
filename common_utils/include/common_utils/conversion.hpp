#ifndef CARPI_COMMON_UTILS_CONVERSION_HPP
#define CARPI_COMMON_UTILS_CONVERSION_HPP

#include <vector>
#include <string>

namespace carpi::utils {
    std::vector<uint8_t> hex2bytes(const std::string& hex);
    std::string bytes_to_hex(const void* binary, std::size_t length);

    inline std::string bytes_to_hex(const std::vector<uint8_t>& data) {
        return bytes_to_hex(data.data(), data.size());
    }

    std::vector<uint8_t> utf8_to_bytes(const std::string& str);
    std::string bytes_to_utf8(const std::vector<uint8_t>& bytes);

    std::string base64_encode(const std::vector<uint8_t>& bytes);
    inline std::string base64_encode(const std::string& data) {
        return base64_encode(utf8_to_bytes(data));
    }

}

#endif //CARPI_COMMON_UTILS_CONVERSION_HPP
