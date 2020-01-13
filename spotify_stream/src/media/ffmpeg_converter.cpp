#include "ffmpeg_converter.hpp"
#include "media_stream.hpp"

namespace carpi::spotify::media {
    LOGGER_IMPL(FfmpegConverter);

    int ffmpeg_read_callback(void *user_data, uint8_t *buf, int buf_size) {
        return ((FfmpegConverter*) user_data)->handle_read_callback(buf, buf_size);
    }

    int ffmpeg_write_callback(void *user_data, uint8_t *buf, int buf_size) {
        return ((FfmpegConverter*) user_data)->handle_write_callback(buf, buf_size);
    }

    FfmpegConverter::FfmpegConverter(std::shared_ptr<MediaStream> upstream) : _upstream{std::move(upstream)} {
        if(!_upstream->read_supported()) {
            log->error("FFMPEG input media stream does not support reading, this cannot work");
            throw std::runtime_error{"Input stream is not readable"};
        }

        load_io_contexts();
    }

    void FfmpegConverter::load_io_contexts() {
        _read_context = std::shared_ptr<AVIOContext>(
                avio_alloc_context(
                        _read_buffer,
                        sizeof _read_buffer,
                        0,
                        this,
                        ffmpeg_read_callback,
                        nullptr,
                        nullptr
                ),
                [](AVIOContext *ctx) {
                    avio_context_free(&ctx);
                }
        );

        _write_context = std::shared_ptr<AVIOContext>(
                avio_alloc_context(
                        _write_buffer,
                        sizeof _write_buffer,
                        1,
                        this,
                        nullptr,
                        ffmpeg_write_callback,
                        nullptr
                ),
                [](AVIOContext *ctx) {
                    avio_context_free(&ctx);
                }
        );
    }

    int FfmpegConverter::handle_read_callback(uint8_t *out_data, int size) {
        return 0;
    }

    int FfmpegConverter::handle_write_callback(uint8_t *in_data, int size) {
        return 0;
    }
}
