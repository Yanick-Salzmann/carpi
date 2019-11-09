#include "camera_stream.hpp"
#include <libyuv.h>

namespace carpi::io::camera {
    LOGGER_IMPL(CameraStream);

    CameraStream::CameraStream() {
        _data_buffer.resize(CAMERA_WIDTH * CAMERA_HEIGHT);
        log->info("LibYUV version: {}", LIBYUV_VERSION);
        start_camera_streaming();
    }

    CameraStream::~CameraStream() {
        _stream->stop_capture();
    }

    void CameraStream::start_camera_streaming() {
        _stream = std::make_shared<video::RawCameraStream>([this](const std::vector<uint8_t> &data, std::size_t size) {
            handle_camera_frame(data, size);
        });

        _stream->initialize_camera({CAMERA_WIDTH, CAMERA_HEIGHT, 1, CAMERA_FPS});
    }

    void CameraStream::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        const auto y = data.data();
        const auto u = y + CAMERA_WIDTH * CAMERA_HEIGHT;
        const auto v = u + (CAMERA_WIDTH * CAMERA_HEIGHT) / 4;

        const auto stride_y = CAMERA_WIDTH;
        const auto stride_u = CAMERA_WIDTH / 2;
        const auto stride_v = CAMERA_WIDTH / 2;

        {
            std::lock_guard<std::mutex> l{_data_lock};
            libyuv::I420ToARGB(y, stride_y, u, stride_u, v, stride_v,(uint8_t *) _data_buffer.data(), CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);
        }
    }
}
