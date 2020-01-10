#include "media_source.hpp"

namespace carpi::spotify::media {

    MediaSource::MediaSource(MediaMetaData meta_data, drm::WidevineAdapter &drm) : _drm_frontend{drm}, _meta_data{std::move(meta_data)} {
        _drm_session = _drm_frontend.create_session(_meta_data.pssh_box);
        _drm_session->wait_for_license();
    }
}