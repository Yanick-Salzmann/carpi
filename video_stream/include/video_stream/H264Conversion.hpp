#ifndef CARPI_VIDEO_STREAM_CONVERSIONCONTEXT_HPP
#define CARPI_VIDEO_STREAM_CONVERSIONCONTEXT_HPP

#include <string>
#include <list>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
}

#include <common_utils/log.hpp>

namespace carpi::video {
    class H264Stream;

    class H264Conversion {
        LOGGER;

        std::shared_ptr<AVFormatContext> _format_context = nullptr;
        std::shared_ptr<AVCodecContext> _codec_context = nullptr;

        std::thread _converter_thread;
        bool _is_running = true;

        std::shared_ptr<AVIOContext> _io_context;
        std::shared_ptr<unsigned char> _io_buffer;
        std::size_t _io_buffer_size;

        std::shared_ptr<H264Stream> _stream;
        std::function<void(void *, std::size_t)> _write_callback;
        std::function<void()> _complete_callback;

        void process_conversion();

        static int on_write_data(void* ptr, uint8_t* data, int size);

        static void log_ffmpeg_message(void *, int, const char *, va_list);

    public:
        explicit H264Conversion(
                std::shared_ptr<H264Stream> stream,
                const std::string &output_extension,
                std::function<void(void *, std::size_t)> write_callback,
                std::function<void()> complete_callback
        );

        ~H264Conversion();

        H264Conversion(H264Conversion &) = delete;

        H264Conversion(H264Conversion &&) = delete;

        void operator=(H264Conversion &) = delete;

        void operator=(H264Conversion &&) = delete;

        static void initialize_ffmpeg();
    };
}

#endif //CARPI_VIDEO_STREAM_CONVERSIONCONTEXT_HPP
