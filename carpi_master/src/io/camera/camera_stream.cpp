#include "camera_stream.hpp"
#include <libyuv.h>
#include <libbase64.h>
#include <sys/shm.h>
#include <common_utils/error.hpp>

namespace carpi::io::camera {
    LOGGER_IMPL(CameraStream);

    class MutexLocker {
        pthread_mutex_t* _mutex;
    public:
        explicit MutexLocker(pthread_mutex_t* mutex) : _mutex(mutex) {
            pthread_mutex_lock(mutex);
        }

        ~MutexLocker() {
            pthread_mutex_unlock(_mutex);
        }

        MutexLocker(const MutexLocker&) = delete;
        MutexLocker(MutexLocker&&) = delete;

        void operator = (const MutexLocker&) = delete;
        void operator = (MutexLocker&&) = delete;
    };

    CameraStream::CameraStream() {
        log->info("LibYUV version: {}", LIBYUV_VERSION);
        start_camera_streaming();
    }

    CameraStream::~CameraStream() {
        _stream->stop_capture();
        std::this_thread::sleep_for(std::chrono::seconds{1});
        shmdt(_video_shmem_mutex);
        shmdt(_camera_frame_buffer);

        shmctl(_mutex_shm_id, IPC_RMID, nullptr);
        shmctl(_camera_shm_id, IPC_RMID, nullptr);
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
            MutexLocker l{_video_shmem_mutex};
            libyuv::I420ToARGB(y, stride_y, u, stride_u, v, stride_v, (uint8_t *) _camera_frame_buffer, CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);
        }
    }

    void CameraStream::begin_capture() {
        _stream->start_capture();
    }

    void CameraStream::init_shared_memory() {
        pthread_mutexattr_t attr{};
        if(pthread_mutexattr_init(&attr) < 0) {
            log->error("Error initializing mutex attribute: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error initializing mutex attribute"};
        }

        if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) < 0) {
            log->error("Error setting shared mutex attribute: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting shared mutex attribute"};
        }

        if((_mutex_shm_id = shmget(SHMEM_KEY_MUTEX, sizeof(pthread_mutex_t), IPC_CREAT)) == -1) {
            log->error("Error creating shared memory for video mutex: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        if((_video_shmem_mutex = (pthread_mutex_t*) shmat(_mutex_shm_id, nullptr, 0)) == -1) {
            log->error("Error creating shared memory for video data: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        pthread_mutex_init(_video_shmem_mutex, &attr);
        pthread_mutexattr_destroy(&attr);

        _camera_shm_id = shmget(SHMEM_KEY_DATA, CAMERA_WIDTH * CAMERA_HEIGHT * 4, IPC_CREAT);
        _camera_frame_buffer = shmat(_camera_shm_id, nullptr, 0);
    }
}
