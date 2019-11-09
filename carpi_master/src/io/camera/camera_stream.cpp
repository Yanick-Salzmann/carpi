#include "camera_stream.hpp"

namespace carpi::io::camera {
    LOGGER_IMPL(CameraStream);

    CameraStream::CameraStream() {
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
        const uint8_t *src_data[] = {data.data()};
        auto *dst_data = (uint8_t *) _data_buffer.data();

        const int src_strides[] = {(int) std::ceil((CAMERA_WIDTH * 6) / 8)};
        const int dst_stride = CAMERA_WIDTH * 4;

        {
            std::lock_guard<std::mutex> l{_data_lock};
            sws_scale(_sws_context, src_data, src_strides, 0, CAMERA_HEIGHT, &dst_data, &dst_stride);
        }

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
        int fd = open("output_image.bmp", O_SYNC | O_CREAT | O_TRUNC);
        write(fd, full_data.data(), full_data.size());
        close(fd);
    }

    CameraStream::~CameraStream() {

    }
}
