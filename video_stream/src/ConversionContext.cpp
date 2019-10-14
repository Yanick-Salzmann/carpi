#include "video_stream/ConversionContext.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
};

namespace carpi::video {
    LOGGER_IMPL(ConversionContext);

    ConversionContext::ConversionContext(const std::string &output_extension) {
        _output_format = av_guess_format(output_extension.c_str(), nullptr, nullptr);
        if (_output_format == nullptr) {
            log->error("Error determining output format for extension '{}'", output_extension);
            throw std::runtime_error{"Error getting output format from extension"};
        }

        const auto rc = avformat_alloc_output_context2(&_format_context, _output_format, nullptr, nullptr);
        if(rc < 0) {
            char error_buffer[AV_ERROR_MAX_STRING_SIZE]{};
            av_make_error_string(error_buffer, AV_ERROR_MAX_STRING_SIZE, rc);
            error_buffer[AV_ERROR_MAX_STRING_SIZE - 1] = '\0';

            log->error("Error creating ffmpeg output context: {}", std::string{error_buffer});
            throw std::runtime_error{"Error creating ffmpeg output context"};
        }
    }

    ConversionContext::~ConversionContext() {
        if(_format_context != nullptr) {
            avformat_free_context(_format_context);
        }
    }

    void ConversionContext::initialize_ffmpeg() {
        avcodec_register_all();
        av_register_all();
        av_log_set_callback(ConversionContext::log_ffmpeg_message);
#ifndef NDEBUG
        av_log_set_level(AV_LOG_DEBUG);
#else
        av_log_set_level(AV_LOG_INFO);
#endif

        log->info("AVCodec:  {}", LIBAVCODEC_IDENT);
        log->info("AVFormat: {}", LIBAVFORMAT_IDENT);
    }

    void ConversionContext::log_ffmpeg_message(void *, int level, const char *format, va_list vargs) {
        static utils::log::Logger logger{"ffmpeg"};
        char log_buffer[2048]{};
        vsnprintf(log_buffer, sizeof log_buffer, format, vargs);
        switch (level) {
            case AV_LOG_FATAL:
            case AV_LOG_PANIC: {
                log->critical(log_buffer);
                break;
            }

            case AV_LOG_ERROR: {
                logger->error(log_buffer);
                break;
            }

            case AV_LOG_WARNING: {
                logger->warn(log_buffer);
                break;
            }

            case AV_LOG_INFO: {
                logger->info(log_buffer);
                break;
            }

            case AV_LOG_VERBOSE:
            case AV_LOG_DEBUG: {
                logger->debug(log_buffer);
                break;
            }

            case AV_LOG_TRACE: {
                logger->trace(log_buffer);
                break;
            }

            default: {
                logger->info(log_buffer);
                break;
            }
        }
    }
}