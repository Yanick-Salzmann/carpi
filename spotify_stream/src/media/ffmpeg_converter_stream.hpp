#ifndef CARPI_FFMPEG_CONVERTER_STREAM_HPP
#define CARPI_FFMPEG_CONVERTER_STREAM_HPP

#include "media_stream.hpp"

#include <mutex>
#include <condition_variable>
#include <vector>


namespace carpi::spotify::media {
    class FfmpegConverterStream : public MediaStream {
        std::condition_variable _read_event{};
        std::mutex _data_lock;

        bool _is_end_of_file = false;

        std::vector<uint8_t> _data{};
    public:
        std::size_t read(void *buffer, std::size_t size) override;

        std::size_t write(const void *buffer, std::size_t size) override;

        bool read_supported() const override;

        bool write_supported() const override;
    };
}

#endif //CARPI_FFMPEG_CONVERTER_STREAM_HPP
