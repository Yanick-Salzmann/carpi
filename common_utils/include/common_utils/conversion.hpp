#ifndef CARPI_COMMON_UTILS_CONVERSION_HPP
#define CARPI_COMMON_UTILS_CONVERSION_HPP

#include <vector>
#include <string>

namespace carpi::utils {
    std::vector<uint8_t> hex2bytes(const std::string& hex);
}

#endif //CARPI_COMMON_UTILS_CONVERSION_HPP
