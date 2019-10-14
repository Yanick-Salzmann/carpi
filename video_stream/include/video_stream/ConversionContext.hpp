#ifndef CARPI_CONVERSIONCONTEXT_HPP
#define CARPI_CONVERSIONCONTEXT_HPP

#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

#include <common_utils/log/Logger.hpp>

namespace carpi::video {
    class ConversionContext {
        LOGGER;

        AVFormatContext* _format_context = nullptr;
        AVOutputFormat* _output_format = nullptr;

        static void log_ffmpeg_message(void*, int, const char*, va_list);

    public:
        explicit ConversionContext(const std::string& output_extension);

        ~ConversionContext();

        ConversionContext(ConversionContext&) = delete;
        ConversionContext(ConversionContext&&) = delete;

        void operator = (ConversionContext&) = delete;
        void operator = (ConversionContext&&) = delete;

        static void initialize_ffmpeg();
    };
}

#endif //CARPI_CONVERSIONCONTEXT_HPP
