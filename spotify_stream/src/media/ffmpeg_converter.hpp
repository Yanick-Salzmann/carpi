#ifndef CARPI_FFMPEG_CONVERTER_HPP
#define CARPI_FFMPEG_CONVERTER_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
};

#include <memory>
#include <common_utils/log.hpp>

namespace carpi::spotify::media {
    class MediaStream;

    class FfmpegConverter {
        LOGGER;

        friend int ffmpeg_write_callback(void *, uint8_t *, int);

        friend int ffmpeg_read_callback(void *, uint8_t *, int);

        uint8_t _read_buffer[4096]{};
        uint8_t _write_buffer[4096]{};

        std::shared_ptr<AVIOContext> _read_context{};
        std::shared_ptr<AVIOContext> _write_context{};

        std::shared_ptr<MediaStream> _upstream;

        int handle_read_callback(uint8_t *out_data, int size);

        int handle_write_callback(uint8_t *in_data, int size);

        void load_io_contexts();

    public:
        FfmpegConverter(std::shared_ptr<MediaStream> upstream);
    };
}

#endif //CARPI_FFMPEG_CONVERTER_HPP
