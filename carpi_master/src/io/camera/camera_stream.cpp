#include "camera_stream.hpp"
#include <libyuv.h>
#include <libbase64.h>
#include <sys/shm.h>
#include <common_utils/error.hpp>
#include <sstream>

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
        {
            MutexLocker l{_video_shmem_mutex};
            //libyuv::I420ToARGB(y, stride_y, u, stride_u, v, stride_v, (uint8_t *) _camera_frame_buffer, CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);
            memcpy(_camera_frame_buffer, data.data(), CAMERA_WIDTH * CAMERA_HEIGHT * 4);
        }

        static std::size_t FRAME_COUNT = 0;

#pragma pack(push, 1)
        struct BitmapHeader {
            uint16_t header = 0x4D42;
            uint32_t size;
            uint16_t r0 = 0;
            uint16_t r1 = 0;
            uint32_t ofs_data = 54;
        };

        struct BitmapInfo {
            uint32_t size = 40;
            uint32_t width;
            int32_t height;
            uint16_t planes = 1;
            uint16_t bitCount = 32;
            uint32_t compression = 0;
            uint32_t size_image;
            uint32_t xpels = 0;
            uint32_t ypels = 0;
            uint32_t clr = 0;
            uint32_t clr_imp = 0;
        };
#pragma pack(pop)

        BitmapHeader header{
                .size = 54 + CAMERA_WIDTH * CAMERA_HEIGHT * 4
        };

        BitmapInfo info{
                .width = CAMERA_WIDTH,
                .height = -(int32_t) CAMERA_HEIGHT,
                .size_image = 0
        };

        std::stringstream strm;
        strm << "./images/image_" << (FRAME_COUNT++) << ".bmp";
        FILE* f = fopen(strm.str().c_str(), "wb");
        fwrite(&header, sizeof header, 1, f);
        fwrite(&info, sizeof info, 1, f);
        fwrite(data.data(), 1, CAMERA_WIDTH * CAMERA_HEIGHT * 4, f);
        fflush(f);
        fclose(f);
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

        if((_mutex_shm_id = shmget(SHMEM_KEY_MUTEX, sizeof(pthread_mutex_t), IPC_CREAT | 0777)) == -1) {
            log->error("Error creating shared memory for video mutex: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        if((_video_shmem_mutex = (pthread_mutex_t*) shmat(_mutex_shm_id, nullptr, 0)) == (void*) -1) {
            log->error("Error locking shared memory for video mutex: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating shared memory"};
        }

        pthread_mutex_init(_video_shmem_mutex, &attr);
        pthread_mutexattr_destroy(&attr);

        _camera_shm_id = shmget(SHMEM_KEY_DATA, CAMERA_WIDTH * CAMERA_HEIGHT * 4, IPC_CREAT | 0777);
        _camera_frame_buffer = shmat(_camera_shm_id, nullptr, 0);
    }
}
