#include "common_utils/error.hpp"

#include <cstring>

namespace carpi::utils {
    std::string error_to_string(int error_number) {
        char error_buffer[1024]{};
#if !_GNU_SOURCE
        strerror_r(error_number, error_buffer, sizeof error_buffer);
        return error_buffer;
#else
        return strerror_r(error_number, error_buffer, sizeof error_buffer);
#endif
    }
}