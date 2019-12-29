#ifndef CARPI_COMMON_UTILS_RANDOM_HPP
#define CARPI_COMMON_UTILS_RANDOM_HPP

#include <random>

namespace carpi::utils {
    std::vector<uint8_t> random_bytes(std::size_t num_bytes);
    uint32_t random_uint32();
}

#endif //CARPI_COMMON_UTILS_RANDOM_HPP
