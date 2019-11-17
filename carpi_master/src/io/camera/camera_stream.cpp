#include "camera_stream.hpp"
#include <sys/shm.h>
#include <common_utils/error.hpp>
#include <sstream>
#include <ui/event_manager.hpp>

namespace carpi::io::camera {
    LOGGER_IMPL(CameraStream);

    class MutexLocker {
        pthread_mutex_t *_mutex;
    public:
        explicit MutexLocker(pthread_mutex_t *mutex) : _mutex(mutex) {
            pthread_mutex_lock(mutex);
        }

        ~MutexLocker() {
            pthread_mutex_unlock(_mutex);
        }

        MutexLocker(const MutexLocker &) = delete;

        MutexLocker(MutexLocker &&) = delete;

        void operator=(const MutexLocker &) = delete;

        void operator=(MutexLocker &&) = delete;
    };

    CameraStream::CameraStream() {
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
        {
            MutexLocker l{_video_shmem_mutex};
            memcpy(((uint8_t *) _camera_frame_buffer) + 4, data.data(), CAMERA_WIDTH * CAMERA_HEIGHT * 4);
            uint16_t w = CAMERA_WIDTH;
            uint16_t h = CAMERA_HEIGHT;
            memcpy(_camera_frame_buffer, &w, 2);
            memcpy(((uint8_t *) _camera_frame_buffer) + 2, &h, 2);
        }
    }

    void CameraStream::begin_capture() {
        {
            std::lock_guard<std::mutex> l{_load_lock};
            if(!_is_initialized) {
                _is_initialized = true;
                start_camera_streaming();
            }
        }

        _stream->start_capture();
    }

    void CameraStream::init_shared_memory() {
        pthread_mutexattr_t attr{};
        if (pthread_mutexattr_init(&attr) < 0) {
            log->error("Error initializing mutex attribute: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error initializing mutex attribute"};
        }

        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) < 0) {
            log->error("Error setting shared mutex attribute: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting shared mutex attribute"};
        }

        if ((_mutex_shm_id = shmget(SHMEM_KEY_MUTEX, sizeof(pthread_mutex_t), IPC_CREAT | 0777)) == -1) {
            log->error("Error creating shared memory for video mutex: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        if ((_video_shmem_mutex = (pthread_mutex_t *) shmat(_mutex_shm_id, nullptr, 0)) == (void *) -1) {
            log->error("Error locking shared memory for video mutex: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        pthread_mutex_init(_video_shmem_mutex, &attr);
        pthread_mutexattr_destroy(&attr);

        _camera_shm_id = shmget(SHMEM_KEY_DATA, SHMEM_SIZE, IPC_CREAT | 0777);
        _camera_frame_buffer = shmat(_camera_shm_id, nullptr, 0);
    }

    void CameraStream::init_events() {
        struct CamParamsResponse {
            uint32_t width;
            uint32_t height;

            [[nodiscard]] nlohmann::json to_json() const {
                return nlohmann::json{
                        {"width",  width},
                        {"height", height}
                };
            }
        };

        struct CamCaptureResponse {
            bool success;

            [[nodiscard]] nlohmann::json to_json() const {
                return nlohmann::json{
                        {"success", success}
                };
            }
        };

        sUiEventMgr->register_event_handler<ui::NoOp, CamParamsResponse>("camera_parameters", [this](const auto &) {
            uint32_t fps;
            CamParamsResponse response{};
            camera_parameters(response.width, response.height, fps);
            return response;
        });

        sUiEventMgr->register_event_handler<ui::NoOp, CamCaptureResponse>("camera_capture", [this](const auto &) {
            this->begin_capture();
            return CamCaptureResponse{
                    .success =true
            };
        });
    }
}
