#ifndef CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP
#define CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP

#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_util.h"

#include <mutex>

#include <common_utils/log.hpp>

namespace carpi::video {
    enum class VideoFormat {
        H264
    };

    class CameraConfiguration {
        uint32_t _width;
        uint32_t _height;

        uint32_t _preview_fps;
        uint32_t _fps;

    public:
        CameraConfiguration(uint32_t width, uint32_t height, uint32_t preview_fps, uint32_t fps) : _width(width), _height(height), _preview_fps(preview_fps), _fps(fps) {}

        [[nodiscard]] uint32_t width() const {
            return _width;
        }

        CameraConfiguration& width(uint32_t width) {
            _width = width;
            return *this;
        }

        [[nodiscard]] uint32_t height() const {
            return _height;
        }

        CameraConfiguration& height(uint32_t height) {
            _height = height;
            return *this;
        }

        [[nodiscard]] uint32_t preview_fps() const {
            return _preview_fps;
        }

        CameraConfiguration& preview_fps(uint32_t preview_fps) {
            _preview_fps = preview_fps;
            return *this;
        }

        [[nodiscard]] uint32_t fps() const {
            return _fps;
        }

        CameraConfiguration& fps(uint32_t fps) {
            _fps = fps;
            return *this;
        }
    };

    class RawCameraStream {
        LOGGER;

        std::mutex _data_read_lock{};
        std::condition_variable _data_variable;

        std::shared_ptr<MMAL_POOL_T> _video_pool{};

        std::shared_ptr<MMAL_COMPONENT_T> _camera{};
        MMAL_PORT_T* _video_port = nullptr;

        std::vector<uint8_t> _buffer_data{};

        static uint32_t map_format(VideoFormat format);

        static void camera_control_callback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
        static void video_data_callback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);

        void handle_camera_control(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
        void handle_video_data(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer, std::shared_ptr<MMAL_BUFFER_HEADER_T>& buffer_ptr);

    public:
        void initialize_camera(const CameraConfiguration& configuration);

        bool start_capture();
    };
};

#endif //CARPI_CAMERA_SLAVE_RAWCAMERASTREAM_HPP
