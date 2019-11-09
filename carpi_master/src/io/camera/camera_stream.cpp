#include "camera_stream.hpp"
#include <libyuv.h>

namespace carpi::io::camera {
    LOGGER_IMPL(CameraStream);

    CameraStream::CameraStream() {
        _data_buffer.resize(CAMERA_WIDTH * CAMERA_HEIGHT);
        log->info("Libswscale version: {}", LIBSWSCALE_IDENT);
        _sws_context = sws_getContext(CAMERA_WIDTH, CAMERA_HEIGHT, AV_PIX_FMT_YUV420P,
                                      CAMERA_WIDTH, CAMERA_HEIGHT, AV_PIX_FMT_RGBA, 0, nullptr, nullptr, nullptr);

        start_camera_streaming();
    }

    void CameraStream::start_camera_streaming() {
        _stream = std::make_shared<video::RawCameraStream>([this](const std::vector<uint8_t> &data, std::size_t size) {
            handle_camera_frame(data, size);
        });

        _stream->initialize_camera({CAMERA_WIDTH, CAMERA_HEIGHT, 1, CAMERA_FPS});
    }

    void CameraStream::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        log->info("Conversion...");
        libyuv::I420ToARGB(data.data(), 0, data.data(), 0, data.data(), 0,
                (uint8_t *) _data_buffer.data(), CAMERA_WIDTH * 4, CAMERA_WIDTH, CAMERA_HEIGHT);

        struct BitmapHeader {
            uint16_t header = 'BM';
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

        BitmapHeader header{
                .size = 54 + CAMERA_WIDTH * CAMERA_HEIGHT * 4
        };

        BitmapInfo info{
                .width = CAMERA_WIDTH,
                .height = -(int32_t) CAMERA_HEIGHT,
                .size_image = CAMERA_WIDTH * CAMERA_HEIGHT * 4
        };

        std::vector<uint8_t> full_data;
        full_data.resize(54 + _data_buffer.size());
        memcpy(&full_data[0], &header, sizeof header);
        memcpy(&full_data[sizeof header], &info, sizeof info);
        memcpy(&full_data[sizeof header + sizeof info], _data_buffer.data(), _data_buffer.size());
        FILE* f = fopen("output_image.bmp", "wb");
        fwrite(&header, sizeof header, 1, f);
        fwrite(&info, sizeof info, 1, f);
        fwrite(_data_buffer.data(), 4, _data_buffer.size(), f);
        fflush(f);
        fclose(f);

        std::this_thread::sleep_for(std::chrono::seconds{5});
    }

    CameraStream::~CameraStream() {

    }
}
