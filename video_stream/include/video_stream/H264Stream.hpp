#ifndef CARPI_VIDEO_STREAM_H264STREAM_HPP
#define CARPI_VIDEO_STREAM_H264STREAM_HPP

#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <common_utils/log.hpp>

namespace carpi::video {
    class IStreamSource {
    public:
        virtual ~IStreamSource() = default;

        virtual std::size_t read(void* buffer, std::size_t num_bytes) = 0;
    };

    class H264Stream {
        LOGGER;

        std::shared_ptr<AVFormatContext> _format_context;
        std::shared_ptr<AVIOContext> _io_context;
        std::shared_ptr<unsigned char> _io_buffer;
        std::size_t _io_buffer_size;

        uint32_t _width;
        uint32_t _height;
        uint32_t _fps;

        std::shared_ptr<IStreamSource> _stream_source;

        static int on_read_buffer(void* ptr, uint8_t* buffer, int size);

    public:
        explicit H264Stream(std::shared_ptr<IStreamSource> stream_source, uint32_t width, uint32_t height, uint32_t fps);

        bool read_next_packet(AVPacket& out_packet);

        [[nodiscard]] uint32_t width() const { return _width; }
        [[nodiscard]] uint32_t height() const { return _height; }
        [[nodiscard]] uint32_t fps() const { return _fps; }
    };
}

#endif //CARPI_VIDEO_STREAM_H264STREAM_HPP
