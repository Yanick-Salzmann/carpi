#ifndef CARPI_MEDIA_PLAYER_HPP
#define CARPI_MEDIA_PLAYER_HPP

/**
 *   this._frontPaddingDuration = 1024 / 44100,
     this._endPaddingDuration = (1024 - lastByteSize % 1024 + 2048) / n

     segments = [ byte_size, time_size (/ timescale) ]
     offset = first byte offset
 */

#include "../drm/widevine_adapter.hpp"
#include <nlohmann/json.hpp>
#include <common_utils/log.hpp>
#include "media_source.hpp"

namespace carpi::spotify::media {
    class MediaPlayer {
        LOGGER;

        net::HttpClient _client{};

        drm::WidevineAdapter& _drm_module;

        std::vector<uint8_t> _pssh_box;
        std::size_t _time_scale = 0;
        std::size_t _padding_samples = 0;
        std::size_t _encoder_delay_samples = 0;
        std::size_t _offset = 0;
        std::vector<std::size_t> _index_range{};
        std::vector<std::pair<std::size_t, std::size_t>> _segments{};

        std::string _file_url{};
        std::string _access_token;

        std::shared_ptr<MediaSource> _media_source{};

        bool load_seek_table(const std::string& song_id);
        void load_song_from_id(const std::string& song_id);

        bool load_cdn(const std::string& song_id);

    public:
        explicit MediaPlayer(std::string access_token, drm::WidevineAdapter& drm) : _access_token{std::move(access_token)}, _drm_module{drm} {

        }

        void play_song(nlohmann::json track_data);
    };
}

#endif //CARPI_MEDIA_PLAYER_HPP
