#ifndef CARPI_COMMON_UTILS_ERROR_HPP
#define CARPI_COMMON_UTILS_ERROR_HPP

#include <string>

namespace carpi::utils {
    std::string error_to_string(int error_number = errno);
}

#endif //CARPI_COMMON_UTILS_ERROR_HPP
