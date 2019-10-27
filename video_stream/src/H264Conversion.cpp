#include "video_stream/H264Conversion.hpp"
#include "video_stream/ErrorHelper.hpp"
#include "video_stream/H264Stream.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
};

#include <unistd.h>

namespace carpi::video {
    LOGGER_IMPL(H264Conversion);

    H264Conversion::H264Conversion(
            std::shared_ptr<H264Stream> stream,
            const std::string &output_extension,
            std::function<void(void *, std::size_t)> write_callback,
            std::function<void()> complete_callback
    ) : _stream(std::move(stream)),
        _write_callback(std::move(write_callback)),
        _complete_callback(std::move(complete_callback)) {

        _io_buffer_size = getpagesize();

        _io_buffer = std::shared_ptr<unsigned char>((unsigned char *) av_malloc(_io_buffer_size), [](unsigned char *ptr) { av_free(ptr); });

        _io_context = std::shared_ptr<AVIOContext>(
                avio_alloc_context(_io_buffer.get(), _io_buffer_size, 1, this, nullptr, H264Conversion::on_write_data, nullptr),
                [](AVIOContext *ctx) { av_free(ctx); }
        );

        auto output_format = av_guess_format(output_extension.c_str(), nullptr, nullptr);
        if (output_format == nullptr) {
            log->error("Error determining output format for extension '{}'", output_extension);
            throw std::runtime_error{"Error getting output format from extension"};
        }

        AVFormatContext* format_context = nullptr;
        const auto rc = avformat_alloc_output_context2(&format_context, output_format, nullptr, nullptr);
        if (rc < 0) {
            log->error("Error creating ffmpeg output context: {}", av_error_to_string(rc));
            throw std::runtime_error{"Error creating ffmpeg output context"};
        }

        const auto codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        log->info("Output format: {}", codec->name);
        auto video_stream = avformat_new_stream(format_context, codec);
        video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        video_stream->codecpar->width = _stream->width();
        video_stream->codecpar->height = _stream->height();
        video_stream->codecpar->codec_id = AV_CODEC_ID_H264;
        video_stream->codecpar->format = AV_PIX_FMT_YUV420P;
        video_stream->time_base = AVRational{.num = 1, .den = 900000};
        video_stream->r_frame_rate = AVRational{.num = 30, .den = 1};
        video_stream->avg_frame_rate = AVRational {.num = 30, .den = 1};

        AVStream** streams = new AVStream*[1];
        streams[0] = video_stream;
        format_context->nb_streams = 1;
        format_context->streams = streams;

        _format_context = std::shared_ptr<AVFormatContext>(format_context, [](AVFormatContext* ctx) { avformat_free_context(ctx); });

        _format_context->pb = _io_context.get();

        _converter_thread = std::thread{[=]() { process_conversion(); }};
    }

    H264Conversion::~H264Conversion() {
        _is_running = false;
        if(_converter_thread.joinable()) {
            _converter_thread.join();
        }
    }

    void H264Conversion::process_conversion() {
        AVDictionary* dict = nullptr;
        av_dict_set(&dict, "movflags", "frag_keyframe+empty_moov", 0);
        av_dict_set(&dict, "r", "30", 0);
        av_dict_set(&dict, "framerate", "30", 0);

        auto rc = avformat_write_header(_format_context.get(), &dict);
        if(rc < 0) {
            av_dict_free(&dict);
            log->error("Error writing target header: {}", av_error_to_string(rc));
            _complete_callback();
            return;
        }

        //av_dict_free(&dict);

        auto did_complete_regularly = false;

        std::shared_ptr<AVPacket> packet = std::shared_ptr<AVPacket>(av_packet_alloc(), [](AVPacket *packet) { av_packet_free(&packet); });
        while(_is_running) {
            try {
                if (!_stream->read_next_packet(*packet.get())) {
                    did_complete_regularly = true;
                    break;
                }
            } catch (std::exception& e) {
                log->warn("Unexpected termination of H264 stream: {}", std::string{e.what()});
                break;
            }

            av_write_frame(_format_context.get(), packet.get());
            av_packet_unref(packet.get());
        }

        if(did_complete_regularly) {
            av_write_trailer(_format_context.get());
        }

        if(_complete_callback) {
            _complete_callback();
        }
    }

    int H264Conversion::on_write_data(void *ptr, uint8_t *data, int size) {
        ((H264Conversion*) ptr)->_write_callback(data, static_cast<std::size_t>(size));
        return size;
    }

    void H264Conversion::initialize_ffmpeg() {
        av_log_set_callback(H264Conversion::log_ffmpeg_message);
#ifndef NDEBUG
        av_log_set_level(AV_LOG_DEBUG);
#else
        av_log_set_level(AV_LOG_INFO);
#endif

        log->info("AVCodec:  {}", LIBAVCODEC_IDENT);
        log->info("AVFormat: {}", LIBAVFORMAT_IDENT);
    }

    void H264Conversion::log_ffmpeg_message(void *, int level, const char *format, va_list vargs) {
        static utils::Logger logger{"ffmpeg"};
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