#include "camera_stream.hpp"
#include <libyuv.h>
#include <libbase64.h>

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
            libyuv::I420ToARGB(y, stride_y, u, stride_u, v, stride_v, (uint8_t *) _data_buffer.data(), CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);
        }
    }

    std::string CameraStream::buffer_to_base64() {
        std::vector<uint32_t> full_data{};
        {
            std::lock_guard<std::mutex> l{_data_lock};
            full_data = _data_buffer;
        }

        std::vector<char> out_data(full_data.size() * 2);
        std::size_t out_size = 0;
        base64_encode((const char*) full_data.data(), full_data.size() * 4, out_data.data(), &out_size, 0);
        return std::string{out_data.begin(), out_data.begin() + out_size};
    }

    void CameraStream::begin_capture() {
        _stream->start_capture();
    }
}
