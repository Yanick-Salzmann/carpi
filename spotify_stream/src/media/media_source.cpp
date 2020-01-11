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
                                .add_header("Range", fmt::format("bytes=0-{}", _meta_data.offset))
                );

        if(resp.status_code() != 200 && resp.status_code() != 206) {
            log->error("Error fetching MP4 header: {} {} ({})", resp.status_code(), resp.status_text(), utils::bytes_to_utf8(resp.body()));
            return;
        }

        MpegDashHeader header{resp.body()};
    }
}