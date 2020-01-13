#ifndef CARPI_MPEG_DASH_HPP
#define CARPI_MPEG_DASH_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <common_utils/log.hpp>
#include <common_utils/io.hpp>
#include <netinet/in.h>
#include <uuid/uuid.h>

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

        void to_big_endian() {
            creation_time = htonl(creation_time);
            modification_time = htonl(modification_time);
            time_scale = htonl(time_scale);
            duration = htonl(duration);
            preferred_rate = htonl(static_cast<uint32_t>(preferred_rate));
            preferred_volume = htonl(static_cast<uint32_t>(preferred_volume));
            preview_time = htonl(preview_time);
            preview_duration = htonl(preview_duration);
            poster_time = htonl(poster_time);
            selection_time = htonl(selection_time);
            selection_duration = htonl(selection_duration);
            cur_position = htonl(cur_position);
            next_track_id = htonl(next_track_id);
            std::reverse(reserved, reserved + 10);
            std::reverse(flags, flags + 3);
            std::reverse((int32_t*) matrix, (int32_t*) matrix + 9);
        }

        uint32_t utc_creation_time() {
            return -2082844800 + creation_time;
        }

        uint32_t utc_modification_time() {
            return -2082844800 + modification_time;
        }
    };
#pragma pop()

    struct MpegDashChunk {
        uint32_t fourcc;
        utils::BinaryReader data;
    };

    class MpegDashHeader {
        LOGGER;
        std::multimap<uint32_t, MpegDashChunk> _chunks{};

        uint32_t _time_scale;

        bool _is_encrypted = false;
        uint32_t _iv_size = 0;
        uuid_t _key_id;

        bool verify_format();
        bool verify_movie_box();

        bool process_movie_box(utils::BinaryReader& moov);
        bool process_track(utils::BinaryReader& trak);
        bool process_media_info(utils::BinaryReader& mdia);
        bool process_media_metadata(utils::BinaryReader& minf);
        bool process_sample_table(utils::BinaryReader& stbl);
        bool process_sample_description(utils::BinaryReader& stsd);
        bool process_encryption_box(utils::BinaryReader& enca);
        bool process_scheme_info(utils::BinaryReader& sinf);
        bool process_protection_info(utils::BinaryReader& schi);
    public:
        explicit MpegDashHeader(const std::vector<uint8_t>& data);

        [[nodiscard]] uint32_t time_scale() const {
            return _time_scale;
        }

        [[nodiscard]] bool default_ecrypted() const {
            return _is_encrypted;
        }

        [[nodiscard]] uint32_t iv_size() const {
            return _iv_size;
        }

        void key_id(uuid_t& key_id) const {
            memcpy(&key_id, &_key_id, sizeof _key_id);
        }
    };

    struct IV_Data {
        union {
            uint64_t short_data;
            struct {
                uint64_t low;
                uint64_t high;
            } long_data;
        };
    };

    class MpegDashSegment {
        LOGGER;

        MpegDashHeader& _header;

        bool _is_encrypted = false;
        std::vector<IV_Data> _per_sample_iv;
        std::vector<uint32_t> _sample_sizes;

        std::vector<uint8_t> _movie_data{};
        std::vector<uint8_t> _movie_header{};

        std::multimap<uint32_t, MpegDashChunk> _chunks{};

        bool process_movie_header(utils::BinaryReader& moof);
        bool process_track_fragment(utils::BinaryReader& traf);

        bool process_sample_encryption(utils::BinaryReader& senc);
        bool process_track_run(utils::BinaryReader& trun);

    public:
        explicit MpegDashSegment(MpegDashHeader& header, const std::vector<uint8_t>& data);

        [[nodiscard]] bool encrypted() const {
            return _is_encrypted;
        }

        [[nodiscard]] const std::vector<uint8_t>& movie_data() const {
            return _movie_data;
        }

        [[nodiscard]] const std::vector<uint8_t>& movie_header() const {
            return _movie_header;
        }

        [[nodiscard]] const std::vector<IV_Data>& per_sample_iv() const {
            return _per_sample_iv;
        }

        [[nodiscard]] const std::vector<uint32_t>& sample_sizes() const {
            return _sample_sizes;
        }

        [[nodiscard]] uint32_t sample_count() const {
            return static_cast<uint32_t>(_sample_sizes.size());
        }

        void write_movie_header(FILE* file);
        void write_movie_header(std::vector<uint8_t>& buffer);

        void write_movie_data(FILE* f, bool header_only = false);
        void write_movie_data(std::vector<uint8_t>& buffer, bool header_only = false);
    };
}

#endif //CARPI_MPEG_DASH_HPP
