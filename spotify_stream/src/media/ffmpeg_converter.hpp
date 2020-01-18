#ifndef CARPI_FFMPEG_CONVERTER_HPP
#define CARPI_FFMPEG_CONVERTER_HPP

#include <memory>
#include <vector>
#include <thread>
#include <common_utils/log.hpp>
#include "ffmpeg_converter_stream.hpp"

namespace carpi::spotify::media {
    class MediaStream;

    class FfmpegConverter {
        LOGGER;

        std::vector<std::thread> _subprocess_threads{};

        std::shared_ptr<MediaStream> _upstream;
        std::shared_ptr<FfmpegConverterStream> _output_stream;

    public:
        explicit FfmpegConverter(std::shared_ptr<MediaStream> upstream);

        void wait_for_completion();

        [[nodiscard]] std::shared_ptr<FfmpegConverterStream> output_stream() const {
            return _output_stream;
        }
    };
}

#endif //CARPI_FFMPEG_CONVERTER_HPP
