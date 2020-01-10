#include "../cdm/content_decryption_module.hpp"
#include "media_player.hpp"
#include "../api_gateway.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <common_utils/string.hpp>

namespace carpi::spotify::media {
    LOGGER_IMPL(MediaPlayer);

    using nlohmann::json;

    void MediaPlayer::play_song(json track_data) {
        const auto manifest = track_data["manifest"];
        const auto file_ids = manifest["file_ids_mp4"];

        uint32_t max_bit_rate = 0;
        std::string max_file_id{};

        for (const auto &item : file_ids) {
            const auto bit_rate = (uint32_t) item["bitrate"];
            if (bit_rate > max_bit_rate) {
                max_bit_rate = bit_rate;
                max_file_id = item["file_id"];
            }
        }

        log->info("Selected track id: {}", max_file_id);
        if (max_file_id.empty()) {
            log->error("Error loading track, no mp4 file id found");
            return;
        }

        load_song_from_id(max_file_id);
    }

    void MediaPlayer::load_song_from_id(const std::string &song_id) {
        if (!load_seek_table(song_id)) {
            return;
        }

        _media_source = std::make_shared<MediaSource>(
                MediaMetaData{
                    .file_url = _file_url,
                    .pssh_box = _pssh_box,
                    .time_scale = _time_scale,
                    .padding_samples = _padding_samples,
                    .encoder_delay_samples = _encoder_delay_samples,
                    .offset = _offset,
                    .index_range = _index_range,
                    .segments = _segments
                },
                _drm_module
        );
    }

    bool MediaPlayer::load_seek_table(const std::string &song_id) {
        const auto url = fmt::format("{}/{}.json", sApiGateway->seektable_endpoint(), song_id);
        const auto response = _client.execute(net::HttpRequest{"GET", url});
        if (response.status_code() != 200) {
            log->error("Error loading seek table for song {}: {} {} ({})", song_id, response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
            return false;
        }

        const auto table = json::parse(utils::bytes_to_utf8(response.body()));
        _pssh_box = utils::base64_decode(table["pssh"]);
        _time_scale = table["timescale"];
        _padding_samples = table["padding_samples"];
        _encoder_delay_samples = table["encoder_delay_samples"];
        _index_range.clear();
        for (const auto &item : table["index_range"]) {
            _index_range.emplace_back((std::size_t) item);
        }

        _segments.clear();
        for (const auto &segment : table["segments"]) {
            _segments.emplace_back((std::size_t) segment[0], (std::size_t) segment[1]);
        }

        _offset = table["offset"];

        log->info("Metadata: {} PSSH bytes, {} offset from start, {} samples per second", _pssh_box.size(), _offset, _time_scale);

        return load_cdn(song_id);
    }

    bool MediaPlayer::load_cdn(const std::string &song_id) {
        const auto cdn = sApiGateway->resolve_audio_file_cdn(song_id);
        log->info("Audio file CDN resolver: {}", cdn);
        const auto response = _client.execute(net::HttpRequest{"GET", cdn}.add_header("Authorization", fmt::format("Bearer {}", _access_token)));
        if (response.status_code() != 200) {
            log->error("Error resolving CDN for song: {} {} ({})", response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
            return false;
        }

        const auto data = json::parse(response.body());
        if (utils::to_lower((std::string) data["result"]) != "cdn") {
            log->error("Unknown CDN response result '{}' expected 'CDN'", (std::string) data["result"]);
            return false;
        }

        const auto url = data["cdnurl"][0];
        _file_url = url;
        log->info("Audio file CDN URL: {}", _file_url);

        return true;
    }
}
