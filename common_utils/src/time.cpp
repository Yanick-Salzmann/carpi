#include "common_utils/time.hpp"

namespace carpi::utils {

    std::string time_to_iso_string(time_t time) {
        char buffer[1024]{};
        tm local_tm;
        localtime_r(&time, &local_tm);

        strftime(buffer, sizeof buffer, "%F %T", &local_tm);
        buffer[1023] = '\0';
        return buffer;
    }
}