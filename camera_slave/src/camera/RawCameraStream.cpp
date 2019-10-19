#include <common_utils/error.hpp>
#include "RawCameraStream.hpp"

#include "interface/mmal/util/mmal_default_components.h"

namespace carpi::camera {
    LOGGER_IMPL(RawCameraStream);

    void RawCameraStream::initialize_camera() {
        MMAL_COMPONENT_T *camera = nullptr;

        const auto status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
        if (status != MMAL_SUCCESS) {
            log->error("Error creating MMAL camera component: {} (error={})", utils::error_to_string(status), status);
            throw std::runtime_error("Error setting up camera");
        }
    }
}