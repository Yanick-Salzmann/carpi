#ifndef CARPI_CARPI_CAMERA_STREAM_HPP
#define CARPI_CARPI_CAMERA_STREAM_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>

#include <video_stream/RawCameraStream.hpp>
#include <vector>
#include <memory>

namespace carpi::io::camera {
    class CameraStream : public utils::singleton<CameraStream> {
        LOGGER;

        const uint32_t CAMERA_WIDTH = video::RawCameraStream::calculate_width(352);
        const uint32_t CAMERA_HEIGHT = video::RawCameraStream::calculate_height(208);
        static const uint32_t CAMERA_FPS = 60;

        static const uint32_t SHMEM_KEY_MUTEX = 0x42434455;
        static const uint32_t SHMEM_KEY_DATA = 0x42434456;
        static const uint32_t SHMEM_SIZE = 1920 * 1080 * 4 + 4;

        pthread_mutex_t* _video_shmem_mutex = nullptr;
        int32_t _mutex_shm_id = 0;
        int32_t _camera_shm_id = 0;

        void* _camera_frame_buffer;

        bool _is_initialized = false;
        std::mutex _load_lock{};

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

        void begin_capture();

        void init_shared_memory();

        void init_events();
    };
}

#define sCameraStream (carpi::io::camera::CameraStream::instance())

#endif //CARPI_CARPI_CAMERA_STREAM_HPP
