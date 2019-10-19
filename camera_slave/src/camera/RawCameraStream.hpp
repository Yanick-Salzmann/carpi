#ifndef CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP
#define CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP

#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_util.h"

#include <common_utils/log.hpp>

namespace carpi::camera {
    class RawCameraStream {
        LOGGER;

    private:
        void initialize_camera();
    };
};

#endif //CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP
