#ifndef CARPI_COMMON_UTILS_CONVERSION_HPP
#define CARPI_COMMON_UTILS_CONVERSION_HPP

#include <vector>
#include <string>

namespace carpi::utils {
    std::vector<uint8_t> hex2bytes(const std::string& hex);
    std::string bytes2hex(const void* binary, std::size_t length);

    std::string base64_encode(const std::vector<uint8_t>& bytes);

    std::vector<uint8_t> utf8_to_bytes(const std::string& str);
}

#endif //CARPI_COMMON_UTILS_CONVERSION_HPP
