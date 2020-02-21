#include "../cdm/content_decryption_module.hpp"
#include "media_player.hpp"
#include "../api_gateway.hpp"
#include "ffmpeg_converter.hpp"
#include "spotify_media_stream.hpp"
#include "track_downloader.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <common_utils/string.hpp>

namespace carpi::spotify::media {
    LOGGER_IMPL(MediaPlayer);

    using nlohmann::json;

    MediaPlayer::MediaPlayer(std::string access_token, drm::WidevineAdapter &drm) : _access_token{std::move(access_token)}, _drm_module{drm} {
//        sFmodSystem->register_media_system_update_callback([=](auto position) {
//            handle_media_stream_update(position, 0);
//        });
    }

    void MediaPlayer::play_song(json track_data, std::size_t seek_to, bool paused) {
        const auto manifest = track_data["manifest"];
        const auto file_ids = manifest["file_ids_mp4"];

        const auto &metadata = track_data["metadata"];
        _track_metadata.artist = fill_artist(metadata);
        _track_metadata.album_image = find_album_image(metadata);
        _track_metadata.title = metadata["name"];

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

        load_song_from_id(max_file_id, seek_to, paused);
    }

    void MediaPlayer::load_song_from_id(const std::string &song_id, std::size_t seek_to, bool paused) {
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
                _drm_module,
                seek_to
        );

        std::thread t{
                [=]() {
                    track_downloader downloader{_media_source, _track_metadata};
                    downloader.download_to_folder(".").get();

                    FfmpegConverter converter{std::make_shared<SpotifyMediaStream>(_media_source)};
                    sFmodSystem->open_sound(converter.output_stream(), _media_source->start_offset_samples(), paused);
                    converter.wait_for_completion();
                }
        };

        t.join();
    }

    bool MediaPlayer::load_seek_table(const std::string &song_id) {
        const auto url = fmt::format("{}/{}.json", sApiGateway->seektable_endpoint(), song_id);
        const auto response = _client.execute(net::http_request{"GET", url});
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
        const auto response = _client.execute(net::http_request{"GET", cdn}.add_header("Authorization", fmt::format("Bearer {}", _access_token)));
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

    MediaPlayer &MediaPlayer::paused(bool is_paused) {
        _is_paused = is_paused;
        sFmodSystem->paused(is_paused);
        return *this;
    }

    void MediaPlayer::handle_media_stream_update(std::size_t position, std::size_t duration) {
        if (_media_position_callback) {
            _media_position_callback(std::min(position, duration), duration);
        }
    }

    std::string MediaPlayer::fill_artist(const nlohmann::json &metadata) {
        const auto &authors = metadata["authors"];
        if (authors.empty()) {
            return "<Unknown Author>";
        }

        auto is_first = true;
        std::stringstream ret{};

        for (const auto &author: authors) {
            const auto &name = author["name"];
            if (is_first) {
                is_first = false;
            } else {
                ret << ", ";
            }

            ret << utils::replace_all(name, '\"', '_');
        }

        return ret.str();
    }

    std::string MediaPlayer::find_album_image(const nlohmann::json &metadata) {
        const auto &images = metadata["images"];
        if (images.empty()) {
            return "";
        }

        std::size_t max_width{0};
        std::string max_url{};

        for (const auto &image : images) {
            std::size_t width = image["width"];
            if(width < max_width) {
                continue;
            }

            max_width = width;
            max_url = image["url"];
        }

        return max_url;
    }
}
