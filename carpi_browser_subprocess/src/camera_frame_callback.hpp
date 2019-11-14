#ifndef CARPI_CARPI_SUBPROC_CAMERA_FRAME_CALLBACK_HPP
#define CARPI_CARPI_SUBPROC_CAMERA_FRAME_CALLBACK_HPP

#include <sys/shm.h>
#include <include/cef_app.h>

namespace carpi {
    class CameraFrameCallback : public CefV8Handler, public CefV8ArrayBufferReleaseCallback {
    IMPLEMENT_REFCOUNTING(CameraFrameCallback);

        static const uint32_t SHMEM_KEY_MUTEX = 0x42434455;
        static const uint32_t SHMEM_KEY_DATA = 0x42434456;

        static const uint32_t SHMEM_SIZE = 1920 * 1080 * 4 + 4;

        pthread_mutex_t* _video_shmem_mutex = nullptr;
        int32_t _mutex_shm_id = 0;
        int32_t _camera_shm_id = 0;

        uint8_t* _frame_data = new uint8_t[SHMEM_SIZE];

        void* _camera_frame_buffer;

    public:
        CameraFrameCallback();

        bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override;

        void ReleaseBuffer(void *buffer) override {}
    };
}

#endif //CARPI_CARPI_SUBPROC_CAMERA_FRAME_CALLBACK_HPP
