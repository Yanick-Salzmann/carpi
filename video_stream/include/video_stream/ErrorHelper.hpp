#ifndef CARPI_VIDEO_STREAM_ERRORHELPER_HPP
#define CARPI_VIDEO_STREAM_ERRORHELPER_HPP

extern "C" {
#include <libavutil/error.h>
};

namespace carpi::video {
    inline std::string av_error_to_string(int error) {
        char error_buffer[AV_ERROR_MAX_STRING_SIZE]{};
        av_make_error_string(error_buffer, AV_ERROR_MAX_STRING_SIZE, error);
        error_buffer[AV_ERROR_MAX_STRING_SIZE - 1] = '\0';
        return std::string{error_buffer};
    }
}

#endif //CARPI_VIDEO_STREAM_ERRORHELPER_HPP
