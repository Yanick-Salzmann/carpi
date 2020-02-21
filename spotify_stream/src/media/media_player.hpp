#ifndef CARPI_MEDIA_PLAYER_HPP
#define CARPI_MEDIA_PLAYER_HPP

#include "../drm/widevine_adapter.hpp"
#include <nlohmann/json.hpp>
#include <common_utils/log.hpp>
#include "media_source.hpp"
#include "fmod_output.hpp"
#include "track_downloader.hpp"
#include <functional>

namespace carpi::spotify::media {
    class MediaPlayer {
        LOGGER;

        net::http_client _client{};

        drm::WidevineAdapter& _drm_module;

        std::vector<uint8_t> _pssh_box{};
        std::size_t _time_scale = 0;
        std::size_t _padding_samples = 0;
        std::size_t _encoder_delay_samples = 0;
        std::size_t _offset = 0;
        std::vector<std::size_t> _index_range{};
        std::vector<std::pair<std::size_t, std::size_t>> _segments{};

        track_metadata _track_metadata{};

        bool _is_paused = true;

        std::string _file_url{};
        std::string _access_token{};

        std::function<void (std::size_t, std::size_t)> _media_position_callback{};

        std::shared_ptr<MediaSource> _media_source{};

        std::string fill_artist(const nlohmann::json& metadata);
        std::string find_album_image(const nlohmann::json& metadata);

        bool load_seek_table(const std::string& song_id);
        void load_song_from_id(const std::string& song_id, std::size_t seek_to, bool paused);

        bool load_cdn(const std::string& song_id);

        void handle_media_stream_update(std::size_t position, std::size_t duration);

    public:
        explicit MediaPlayer(std::string access_token, drm::WidevineAdapter& drm);

        void set_media_position_update_callback(std::function<void(std::size_t, std::size_t)> callback) {
            _media_position_callback = std::move(callback);
        }

        void play_song(nlohmann::json track_data, std::size_t seek_to, bool paused);

        [[nodiscard]] bool paused() const {
            return _is_paused;
        }

        MediaPlayer& paused(bool is_paused);
    };
}

#endif //CARPI_MEDIA_PLAYER_HPP
