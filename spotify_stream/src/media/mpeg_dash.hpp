#ifndef CARPI_MPEG_DASH_HPP
#define CARPI_MPEG_DASH_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <common_utils/log.hpp>
#include <common_utils/io.hpp>

namespace carpi::spotify::media {
#pragma pack(push, 1)
    struct MovieHeaderBox {
        uint8_t version;
        uint8_t flags[3];
        uint32_t creation_time;
        uint32_t modification_time;
        uint32_t time_scale;
        uint32_t duration;
        int32_t preferred_rate;
        int32_t preferred_volume;
        uint8_t reserved[10];
        int32_t matrix[3][3];
        uint32_t preview_time;
        uint32_t preview_duration;
        uint32_t poster_time;
        uint32_t selection_time;
        uint32_t selection_duration;
        uint32_t cur_position;
        uint32_t next_track_id;
    };
#pragma pop()

    struct MpegDashChunk {
        uint32_t fourcc;
        utils::BinaryReader data;
    };

    class MpegDashHeader {
        LOGGER;
        std::multimap<uint32_t, MpegDashChunk> _chunks{};

        bool verify_format();
        bool verify_movie_box();

        bool process_movie_box(utils::BinaryReader& moov);
    public:
        explicit MpegDashHeader(const std::vector<uint8_t>& data);
    };

    class MpegDashSegment {
        LOGGER;

        std::multimap<uint32_t, MpegDashChunk> _chunks{};

    public:
        explicit MpegDashSegment(const std::vector<uint8_t>& data);
    };
}

#endif //CARPI_MPEG_DASH_HPP
