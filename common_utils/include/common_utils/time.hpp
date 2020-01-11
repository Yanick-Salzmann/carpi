#ifndef CARPI_COMMON_UTILS_TIME_HPP
#define CARPI_COMMON_UTILS_TIME_HPP

#include <ctime>
#include <string>

namespace carpi::utils {
    std::string time_to_iso_string(time_t time);
}

#endif //CARPI_COMMON_UTILS_TIME_HPP
