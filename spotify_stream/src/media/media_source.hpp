#ifndef CARPI_MEDIA_SOURCE_HPP
#define CARPI_MEDIA_SOURCE_HPP

#include "../drm/widevine_adapter.hpp"
#include "../drm/widevine_session.hpp"

#include <common_utils/log.hpp>

namespace carpi::spotify::media {
    struct MediaMetaData {
        std::string file_url;

        std::vector<uint8_t> pssh_box{};
        std::size_t time_scale = 0;
        std::size_t padding_samples = 0;
        std::size_t encoder_delay_samples = 0;
        std::size_t offset = 0;
        std::vector<std::size_t> index_range{};
        std::vector<std::pair<std::size_t, std::size_t>> segments{};
    };

    class MediaSource {
        LOGGER;

        net::HttpClient _client{};

        drm::WidevineAdapter& _drm_frontend;
        std::shared_ptr<drm::WidevineSession> _drm_session;
        MediaMetaData _meta_data;

        void download_to_file();

    public:
        MediaSource(MediaMetaData meta_data, drm::WidevineAdapter& drm);
    };
}

#endif //CARPI_MEDIA_SOURCE_HPP
