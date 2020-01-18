#include <stdexcept>
#include "spotify_media_stream.hpp"
#include "media_source.hpp"

namespace carpi::spotify::media {

    std::size_t SpotifyMediaStream::read(void *buffer, std::size_t size) {
        if(_cur_segment_offset >= _segment_buffer.size() || _segment_buffer.empty()) {
            _segment_buffer = _media_source->read_next_data();
            if(_segment_buffer.empty()) {
                return 0;
            }

            _cur_segment_offset = 0;
        }

        const auto to_read = std::min<std::size_t>(size, _segment_buffer.size() - _cur_segment_offset);
        memcpy(buffer, &_segment_buffer[_cur_segment_offset], to_read);
        _cur_segment_offset += to_read;
        if(_cur_segment_offset >= _segment_buffer.size()) {
            _segment_buffer = _media_source->read_next_data();
            if(_segment_buffer.empty()) {
                return to_read;
            }

            _cur_segment_offset = 0;
        }

        return to_read;
    }

    std::size_t SpotifyMediaStream::write(const void *buffer, std::size_t size) {
        throw std::runtime_error{"Writing to a spotify stream is not supported"};
    }

    bool SpotifyMediaStream::read_supported() const {
        return true;
    }

    bool SpotifyMediaStream::write_supported() const {
        return false;
    }
}