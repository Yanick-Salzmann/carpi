#ifndef CARPI_COMMON_UTILS_RANDOM_HPP
#define CARPI_COMMON_UTILS_RANDOM_HPP

#include <random>

namespace carpi::utils {
    std::vector<uint8_t> random_bytes(std::size_t num_bytes);
}

#endif //CARPI_COMMON_UTILS_RANDOM_HPP
