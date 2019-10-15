#include "video_stream/H264Stream.hpp"
#include "video_stream/ErrorHelper.hpp"

#include <unistd.h>

namespace carpi::video {
    LOGGER_IMPL(H264Stream);

    H264Stream::H264Stream(std::shared_ptr<IStreamSource> stream_source) : _stream_source(std::move(stream_source)) {
        _io_buffer_size = getpagesize();

        _io_buffer = std::shared_ptr<unsigned char>((unsigned char *) av_malloc(_io_buffer_size), [](unsigned char *ptr) { av_free(ptr); });

        _io_context = std::shared_ptr<AVIOContext>(
                avio_alloc_context(_io_buffer.get(), _io_buffer_size, 0, this, H264Stream::on_read_buffer, nullptr, nullptr),
                [](AVIOContext *ctx) { av_free(ctx); }
        );

        _format_context = std::shared_ptr<AVFormatContext>(
                avformat_alloc_context(),
                [](AVFormatContext *ctx) { avformat_free_context(ctx); }
        );

        const auto h264_input_format = av_find_input_format("h264");
        if (h264_input_format == nullptr) {
            log->error("Could not find H264 input parser in FFmpeg");
            throw std::runtime_error("H264 not found");
        }

        _format_context->pb = _io_context.get();

        auto formatPtr = _format_context.get();
        const auto res = avformat_open_input(&formatPtr, "(memory file)", h264_input_format, nullptr);
        if(res != 0) {
            log->error("Error opening input stream: {}", av_error_to_string(res));
            throw std::runtime_error("Error opening input stream");
        }
    }

    int H264Stream::on_read_buffer(void *ptr, uint8_t *buffer, int size) {
        auto stream = (H264Stream *) ptr;
        return static_cast<int>(stream->_stream_source->read(buffer, size));
    }

    bool H264Stream::read_next_packet(AVPacket& out_packet) {
        const auto res = av_read_frame(_format_context.get(), &out_packet);
        if(res == 0) {
            return true;
        }

        if(res == AVERROR_EOF) {
            return false;
        }

        log->error("Error reading frame from H264 input stream: {}", av_error_to_string(res));
        throw std::runtime_error("Error reading frame from H264 stream");
    }
}