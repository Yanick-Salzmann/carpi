#ifndef CARPI_MEDIA_SOURCE_HPP
#define CARPI_MEDIA_SOURCE_HPP

#include "../drm/widevine_adapter.hpp"
#include "../drm/widevine_session.hpp"
#include "mpeg_dash.hpp"

#include <common_utils/log.hpp>
#include <stdexcept>

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

    enum class seek_unit {
        milliseconds,
        seconds,
        samples
    };

    class MediaSource {
        LOGGER;

        net::http_client _client{};

        drm::WidevineAdapter& _drm_frontend;
        std::shared_ptr<drm::WidevineSession> _drm_session;
        MediaMetaData _meta_data;

        std::vector<std::pair<std::size_t, std::size_t>> _segment_table{};

        std::vector<uint8_t> _header_data{};
        std::shared_ptr<MpegDashHeader> _header{};

        bool _header_delivered = false;

        std::size_t _segment_index = 0;
        std::size_t _sample_index = 0;

        std::size_t _seek_position = 0;

        void load_header();

        std::vector<uint8_t> fetch_segment_data(std::size_t index);

        std::vector<uint8_t> read_next_segment();

    public:
        MediaSource(MediaMetaData meta_data, drm::WidevineAdapter& drm, std::size_t seek_offset);

        std::vector<uint8_t> read_next_data();

        [[nodiscard]] std::size_t start_offset_samples() const {
            return _sample_index;
        }

        MediaSource& seek_position(std::size_t seek_position, seek_unit unit = seek_unit::milliseconds);
    };
}

#endif //CARPI_MEDIA_SOURCE_HPP
