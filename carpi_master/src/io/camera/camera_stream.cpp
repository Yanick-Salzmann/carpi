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
        log->info("Conversion...");
        const auto y = data.data();
        const auto u = y + CAMERA_WIDTH * CAMERA_HEIGHT;
        const auto v = u + (CAMERA_WIDTH * CAMERA_HEIGHT) / 4;

        {
            std::lock_guard<std::mutex> l{_data_lock};
            libyuv::I420ToARGB(y, CAMERA_WIDTH, u, CAMERA_WIDTH / 2, v, CAMERA_WIDTH / 2,
                               (uint8_t *) _data_buffer.data(), CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);
        }
    }
}
