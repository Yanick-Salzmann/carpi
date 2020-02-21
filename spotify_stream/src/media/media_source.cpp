#include "media_source.hpp"
#include "mpeg_dash.hpp"

namespace carpi::spotify::media {
    LOGGER_IMPL(MediaSource);

    MediaSource::MediaSource(MediaMetaData meta_data, drm::WidevineAdapter &drm, std::size_t seek_offset) : _drm_frontend{drm}, _meta_data{std::move(meta_data)} {
        _drm_session = _drm_frontend.create_session(_meta_data.pssh_box);
        _drm_session->wait_for_license();

        std::size_t cur_offset = _meta_data.offset;
        for (const auto &pair : _meta_data.segments) {
            _segment_table.emplace_back(cur_offset, pair.first);
            cur_offset += pair.first;
        }

        seek_position(seek_offset, seek_unit::milliseconds);
    }

    std::vector<uint8_t> MediaSource::read_next_data() {
        if (!_header_delivered) {
            load_header();
            _header_delivered = true;
            return _header_data;
        }

        return read_next_segment();
    }

    std::vector<uint8_t> MediaSource::read_next_segment() {
        if (_segment_index >= _meta_data.segments.size()) {
            return {};
        }

        const auto data = fetch_segment_data(_segment_index++);
        if (data.empty()) {
            return {};
        }

        std::vector<uint8_t> segment_data{};
        MpegDashSegment segment{*_header, data};
        segment.write_movie_header(segment_data);

        segment.write_movie_data(segment_data, segment.encrypted());

        if (!segment.encrypted()) {
            return segment_data;
        }

        const auto &movie_data = segment.movie_data();
        const auto sample_count = segment.sample_count();

        auto sample_offset = 0;

        uuid_t key_id{};
        _header->key_id(key_id);

        for (auto sample = 0; sample < sample_count; ++sample) {
            const auto &iv = segment.per_sample_iv()[sample];
            std::vector<uint8_t> decrypted{};
            _drm_session->decrypt_sample(key_id, &iv, _header->iv_size(), &movie_data[sample_offset], segment.sample_sizes()[sample], decrypted);
            if (decrypted.size() != segment.sample_sizes()[sample]) {
                log->info("Encrypted: {}, decrypted: {}", segment.sample_sizes()[sample], decrypted.size());
                throw std::runtime_error{"Encrypted and decrypted size for MP4 sample is not equal. This is not supported (yet)."};
            }

            sample_offset += segment.sample_sizes()[sample];

            segment_data.insert(segment_data.end(), decrypted.begin(), decrypted.end());
        }

        return segment_data;
    }

    void MediaSource::load_header() {
        const auto resp = _client.execute(
                net::http_request{"GET", _meta_data.file_url}
                        .add_header("Range", fmt::format("bytes=0-{}", _meta_data.offset - 1))
        );

        if (resp.status_code() != 200 && resp.status_code() != 206) {
            log->error("Error fetching MP4 header: {} {} ({})", resp.status_code(), resp.status_text(), utils::bytes_to_utf8(resp.body()));
            return;
        }

        _header_data = resp.body();

        _header = std::make_shared<MpegDashHeader>(resp.body());
        if (_header->time_scale() != _meta_data.time_scale) {
            log->warn("Mismatch in timescale between MP4 file and seek table metadata: {} vs {}. This could cause invalid audio", _header->time_scale(), _meta_data.time_scale);
        }

        log->info("MP4 default encryption: {}", _header->default_ecrypted() ? "encrypted" : "not encrypted");
    }

    std::vector<uint8_t> MediaSource::fetch_segment_data(std::size_t index) {
        const auto &pair = _segment_table[index];
        const auto data = _client.execute(net::http_request{"GET", _meta_data.file_url}.add_header("Range", fmt::format("bytes={}-{}", pair.first, pair.first + pair.second - 1)));
        if (data.status_code() != 200 && data.status_code() != 206) {
            log->error("Error fetching MP4 segment: {} {} ({})", data.status_code(), data.status_text(), utils::bytes_to_utf8(data.body()));
            return {};
        }

        return data.body();
    }

    MediaSource &MediaSource::seek_position(std::size_t seek_position, seek_unit unit) {
        if(unit == seek_unit::milliseconds) {
            _seek_position =
                    (std::size_t) (seek_position * 44.1f); // 44100 per second -> 44100 / 1000 per millisecond
        } else if(unit == seek_unit::seconds) {
            _seek_position = seek_position * 44100;
        } else {
            _seek_position = seek_position;
        }

        auto segment_found = false;
        auto cur_time = 0;
        for (auto i = std::size_t{0}; i < _meta_data.segments.size() - 1; ++i) {
            const auto length = _meta_data.segments[i].second;
            if (cur_time <= _seek_position && (cur_time + length) > _seek_position) {
                segment_found = true;
                _segment_index = i;
                break;
            }

            cur_time += length;
        }

        if (!segment_found) {
            _segment_index = static_cast<size_t>(-1);
        } else {
            _sample_index = (_seek_position - cur_time) / 1024;
        }

        return *this;
    }
}