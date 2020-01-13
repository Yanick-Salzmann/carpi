#ifndef CARPI_SPOTIFY_MEDIA_STREAM_HPP
#define CARPI_SPOTIFY_MEDIA_STREAM_HPP

#include <vector>
#include <mutex>
#include <memory>
#include "media_stream.hpp"

namespace carpi::spotify::media {
    class MediaSource;

    class SpotifyMediaStream : public MediaStream {
        std::shared_ptr<MediaSource> _media_source;

        std::size_t _cur_segment_offset = 0;
        std::vector<uint8_t> _segment_buffer{};

    public:
        explicit SpotifyMediaStream(std::shared_ptr<MediaSource> source) : _media_source{std::move(source)} {

        }

        std::size_t read(void *buffer, std::size_t size) override;

        std::size_t write(const void *buffer, std::size_t size) override;

        [[nodiscard]] bool read_supported() const override;

        [[nodiscard]] bool write_supported() const override;
    };
}

#endif //CARPI_SPOTIFY_MEDIA_STREAM_HPP
