#ifndef CARPI_IMAGE_HPP
#define CARPI_IMAGE_HPP

#include <cstdint>
#include <string>

namespace carpi::spotify::web::model {
    struct image {
        uint32_t height;
        uint32_t width;
        std::string url;
    };
}

#endif //CARPI_IMAGE_HPP
