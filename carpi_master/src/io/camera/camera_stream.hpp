#ifndef CARPI_CARPI_CAMERA_STREAM_HPP
#define CARPI_CARPI_CAMERA_STREAM_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>

#include <video_stream/RawCameraStream.hpp>
#include <vector>
#include <memory>

namespace carpi::io::camera {
    class CameraStream : public utils::Singleton<CameraStream> {
        LOGGER;

        const uint32_t CAMERA_WIDTH = video::RawCameraStream::calculate_width(480);
        const uint32_t CAMERA_HEIGHT = video::RawCameraStream::calculate_height(360);
        static const uint32_t CAMERA_FPS = 30;

        std::mutex _data_lock;
        std::vector<uint32_t> _data_buffer;

        std::shared_ptr<video::RawCameraStream> _stream;

        void handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size);

        void start_camera_streaming();

    public:
        CameraStream();

        CameraStream(const CameraStream &) = delete;

        CameraStream(CameraStream &&) = delete;

        ~CameraStream();

        void operator=(const CameraStream &) = delete;

        void operator=(CameraStream &&) = delete;

        void camera_parameters(uint32_t &width, uint32_t &height, uint32_t &fps) {
            width = CAMERA_WIDTH;
            height = CAMERA_HEIGHT;
            fps = CAMERA_FPS;
        }
    };
}

#define sCameraStream (carpi::io::camera::CameraStream::instance())

#endif //CARPI_CARPI_CAMERA_STREAM_HPP
