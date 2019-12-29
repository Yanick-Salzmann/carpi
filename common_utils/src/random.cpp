#include "common_utils/random.hpp"
#include <random>
#include <chrono>

namespace carpi::utils {
    static thread_local std::mt19937 _random_generator{static_cast<unsigned long>(std::chrono::steady_clock::now().time_since_epoch().count())};

    std::vector<uint8_t> random_bytes(std::size_t num_bytes) {
        std::uniform_int_distribution<uint8_t> distribution{};

        std::vector<uint8_t> ret(num_bytes);
        for(auto i = 0; i < num_bytes; ++i) {
            ret[i] = distribution(_random_generator);
        }

        return ret;
    }

    uint32_t random_uint32() {
        return std::uniform_int_distribution<uint32_t>{}(_random_generator);
    }
}