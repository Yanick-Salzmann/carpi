#ifndef CARPI_COMMON_UTILS_CONVERSION_HPP
#define CARPI_COMMON_UTILS_CONVERSION_HPP

#include <vector>
#include <string>

namespace carpi::utils {
    std::vector<uint8_t> hex2bytes(const std::string& hex);
    std::string bytes2hex(const void* binary, std::size_t length);
}

#endif //CARPI_COMMON_UTILS_CONVERSION_HPP
