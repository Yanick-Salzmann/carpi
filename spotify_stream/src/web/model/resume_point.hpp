#ifndef CARPI_RESUME_POINT_HPP
#define CARPI_RESUME_POINT_HPP

#include <cstdint>

namespace carpi::spotify::web::model {
    struct resume_point {
        bool fully_played;
        int32_t resume_position_ms;
    };
}

#endif //CARPI_RESUME_POINT_HPP
