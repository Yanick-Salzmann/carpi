#ifndef CARPI_CARPI_FBRENDERER_HPP
#define CARPI_CARPI_FBRENDERER_HPP

#include <string>

#include <common_utils/log.hpp>

namespace carpi::ui {
    class FbRenderer {
        LOGGER;

        int32_t _device = -1;

    public:
        explicit FbRenderer(const std::string& device = "/dev/fb1");
    };
}

#endif //CARPI_CARPI_FBRENDERER_HPP
