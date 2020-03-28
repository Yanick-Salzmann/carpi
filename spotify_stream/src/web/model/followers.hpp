#ifndef CARPI_FOLLOWERS_HPP
#define CARPI_FOLLOWERS_HPP

#include <string>
#include <cstdint>

namespace carpi::spotify::web::model {
    struct followers {
        std::string href;
        int32_t total;
    };

}

#endif //CARPI_FOLLOWERS_HPP
