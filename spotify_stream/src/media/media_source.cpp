#include "media_source.hpp"
#include "mpeg_dash.hpp"

namespace carpi::spotify::media {
    LOGGER_IMPL(MediaSource);

    MediaSource::MediaSource(MediaMetaData meta_data, drm::WidevineAdapter &drm) : _drm_frontend{drm}, _meta_data{std::move(meta_data)} {
        _drm_session = _drm_frontend.create_session(_meta_data.pssh_box);
        _drm_session->wait_for_license();
        download_to_file();
    }

    void MediaSource::download_to_file() {
        const auto resp = _client.execute(
                        net::HttpRequest{"GET", _meta_data.file_url}
                                .add_header("Range", fmt::format("bytes=0-{}", _meta_data.offset - 1))
                );

        if(resp.status_code() != 200 && resp.status_code() != 206) {
            log->error("Error fetching MP4 header: {} {} ({})", resp.status_code(), resp.status_text(), utils::bytes_to_utf8(resp.body()));
            return;
        }

        MpegDashHeader header{resp.body()};
        if(header.time_scale() != _meta_data.time_scale) {
            log->warn("Mismatch in timescale between MP4 file and seek table metadata: {} vs {}. This could cause invalid audio", header.time_scale(), _meta_data.time_scale);
        }

        log->info("MP4 default encryption: {}", header.default_ecrypted() ? "encrypted" : "not encrypted");

        uuid_t key_id;
        header.key_id(key_id);
        log->info("MP4 DRM key id: {}", utils::bytes_to_hex(key_id, sizeof key_id));
        log->info("MP4 DRM IV size: {}", header.iv_size());

        auto cur_offset = _meta_data.offset;

        FILE* out_file = fopen("file1.mp4", "wb");
        fwrite(resp.body().data(), 1, resp.body().size(), out_file);

        for(auto i = 0; i < _meta_data.segments.size(); ++i) {
            const auto& pair = _meta_data.segments[i];
            const auto data = _client.execute(net::HttpRequest{"GET", _meta_data.file_url}.add_header("Range", fmt::format("bytes={}-{}", cur_offset, cur_offset + pair.first - 1)));
            if(resp.status_code() != 200 && resp.status_code() != 206) {
                log->error("Error fetching MP4 segment: {} {} ({})", resp.status_code(), resp.status_text(), utils::bytes_to_utf8(resp.body()));
                return;
            }

            cur_offset += pair.first;

            MpegDashSegment segment{header, data.body()};
            segment.write_movie_header(out_file);

            log->info("Segment {} is {}", i + 1, segment.encrypted() ? "encrypted" : "not encrypted");
            segment.write_movie_data(out_file, segment.encrypted());

            if(!segment.encrypted()) {
                continue;
            }

            const auto& movie_data = segment.movie_data();
            const auto sample_count = segment.sample_count();

            auto sample_offset = 0;

            for(auto sample = 0; sample < sample_count; ++sample) {
                const auto& iv = segment.per_sample_iv()[sample];
                std::vector<uint8_t> decrypted{};
                _drm_session->decrypt_sample(key_id, &iv, header.iv_size(), &movie_data[sample_offset], segment.sample_sizes()[sample], decrypted);
                if(decrypted.size() != segment.sample_sizes()[sample]) {
                    log->info("Encrypted: {}, decrypted: {}", segment.sample_sizes()[sample], decrypted.size());
                    throw std::runtime_error{"Encrypted and decrypted size for MP4 sample is not equal. This is not supported (yet)."};
                }

                sample_offset += segment.sample_sizes()[sample];

                fwrite(decrypted.data(), 1, decrypted.size(), out_file);
            }
        }

        fclose(out_file);
    }
}