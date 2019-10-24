#ifndef CARPI_COMMON_UTILS_NETWORK_HPP
#define CARPI_COMMON_UTILS_NETWORK_HPP

#include <cstdint>

namespace carpi::utils {
    void read(int socket, void* buffer, std::size_t size);

    template<typename T>
    inline T read(int socket) {
        T ret;
        read(socket, &ret, sizeof ret);
        return ret;
    }
}

#endif //CARPI_COMMON_UTILS_NETWORK_HPP
