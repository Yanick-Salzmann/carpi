#ifndef CARPI_COMMON_UTILS_STRING_HPP
#define CARPI_COMMON_UTILS_STRING_HPP

#include <string>

namespace carpi::utils {
    std::string trim_left(const std::string& value);
    std::string trim_right(const std::string& value);

    std::string trim(const std::string& value) {
        return trim_left(trim_right(value));
    }

    std::string to_lower(const std::string& value);
}

#endif //CARPI_COMMON_UTILS_STRING_HPP
