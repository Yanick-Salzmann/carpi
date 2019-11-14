#include "camera_frame_callback.hpp"

namespace carpi {

    CameraFrameCallback::CameraFrameCallback() {
        pthread_mutexattr_t attr{};
        if(pthread_mutexattr_init(&attr) < 0) {
            throw std::runtime_error{"Error initializing mutex attribute"};
        }

        if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) < 0) {
            throw std::runtime_error{"Error setting shared mutex attribute"};
        }

        if((_mutex_shm_id = shmget(SHMEM_KEY_MUTEX, sizeof(pthread_mutex_t), IPC_CREAT | 0777)) == -1) {
            throw std::runtime_error{"Error creating shared memory"};
        }

        if((_video_shmem_mutex = (pthread_mutex_t*) shmat(_mutex_shm_id, nullptr, 0)) == (void*) -1) {
            throw std::runtime_error{"Error creating shared memory"};
        }

        pthread_mutex_init(_video_shmem_mutex, &attr);
        pthread_mutexattr_destroy(&attr);

        _camera_shm_id = shmget(SHMEM_KEY_DATA, SHMEM_SIZE, IPC_CREAT | 0777);
        _camera_frame_buffer = shmat(_camera_shm_id, nullptr, SHM_RDONLY);
    }

    bool CameraFrameCallback::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) {
        pthread_mutex_lock(_video_shmem_mutex);
        memcpy(_frame_data, _camera_frame_buffer, SHMEM_SIZE);
        pthread_mutex_unlock(_video_shmem_mutex);

        retval = CefV8Value::CreateArrayBuffer(_frame_data, SHMEM_SIZE, this);

        return true;
    }
}