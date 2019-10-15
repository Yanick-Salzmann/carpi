#ifndef CARPI_VIDEO_STREAM_H264STREAM_HPP
#define CARPI_VIDEO_STREAM_H264STREAM_HPP

#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <common_utils/log/Logger.hpp>

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

        std::shared_ptr<IStreamSource> _stream_source;

        static int on_read_buffer(void* ptr, uint8_t* buffer, int size);

    public:
        explicit H264Stream(std::shared_ptr<IStreamSource> stream_source, uint32_t width, uint32_t height, uint32_t fps);

        bool read_next_packet(AVPacket& out_packet);
    };
}

#endif //CARPI_VIDEO_STREAM_H264STREAM_HPP
