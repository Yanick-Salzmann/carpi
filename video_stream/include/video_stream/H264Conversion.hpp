#ifndef CARPI_VIDEO_STREAM_CONVERSIONCONTEXT_HPP
#define CARPI_VIDEO_STREAM_CONVERSIONCONTEXT_HPP

#include <string>
#include <list>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
}

#include <common_utils/log/Logger.hpp>

namespace carpi::video {
    class H264Stream;

    class H264Conversion {
        LOGGER;

        std::shared_ptr<AVFormatContext> _format_context = nullptr;

        std::thread _converter_thread;
        bool _is_running = true;

        std::shared_ptr<H264Stream> _stream;
        std::function<void(void *, std::size_t)> _write_callback;
        std::function<void()> _complete_callback;

        void process_conversion();

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
