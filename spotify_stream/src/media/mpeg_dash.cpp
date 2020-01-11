#include <common_utils/string.hpp>
#include <common_utils/time.hpp>
#include "mpeg_dash.hpp"

namespace carpi::spotify::media {
#define MAKEFOURCC(ch0, ch1, ch2, ch3)  \
    ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8u) |  \
    ((uint32_t)(uint8_t)(ch2) << 16u) | ((uint32_t)(uint8_t)(ch3) << 24u ))

    LOGGER_IMPL(MpegDashHeader);

    std::string fourcc_to_string(uint32_t value) {
        const auto* ptr = (const char*) &value;
        return std::string{ptr, ptr + 4};
    }

    void fourcc_reader(const std::vector<uint8_t> &data, std::multimap<uint32_t, MpegDashChunk> &chunk_map) {
        utils::BinaryReader reader{data};

        while (reader.available() >= 8) {
            const auto size = reader.read<uint32_t, utils::big_endian_policy>();
            const auto signature = reader.read<uint32_t>();

            MpegDashChunk chunk{
                .fourcc = signature
            };


            if(size > 8) {
                std::vector<uint8_t> chunk_data(size - 8);
                reader.read(chunk_data);
                chunk.data = utils::BinaryReader{chunk_data};
            }

            chunk_map.emplace(signature, chunk);
        }
    }

    MpegDashHeader::MpegDashHeader(const std::vector<uint8_t> &data) {
        fourcc_reader(data, _chunks);
        if(!verify_format()) {
            log->error("Unknown MP4 format");
        }

        if(!verify_movie_box()) {
            log->error("MP4 metadata cannot be parsed");
        }
    }

    bool MpegDashHeader::verify_format() {
        const auto itr = _chunks.find(MAKEFOURCC('f', 't', 'y', 'p'));
        if(itr == _chunks.end()) {
            log->info("Chunk 'ftyp' not found");
            return false;
        }

        const auto major_version = itr->second.data.read<uint32_t>();
        const auto minor_version = itr->second.data.read<uint32_t>();

        if(major_version != MAKEFOURCC('i', 's', 'o', 'm')) {
            log->info("Major version of MP4 file is not 'isom', this is not supported (fourcc={:x})", major_version);
            return false;
        }

        const auto num_types = itr->second.data.available() / 4;
        auto has_mpeg_dash = false;

        for(auto i = 0; i < num_types; ++i) {
            const auto brand = fourcc_to_string(itr->second.data.read<uint32_t>());
            log->info("Supported brand for MP4 file: {}", brand);
            if(utils::to_lower(brand) == "dash") {
                has_mpeg_dash = true;
            }
        }

        if(!has_mpeg_dash) {
            log->info("MPEG-DASH is not supported in file");
            return false;
        }

        return true;
    }

    bool MpegDashHeader::verify_movie_box() {
        auto itr = _chunks.find(MAKEFOURCC('m', 'o', 'o', 'v'));
        if(itr == _chunks.end()) {
            log->error("No MOOV chunk found in MP4 header");
            return false;
        }

        return process_movie_box(itr->second.data);
    }

    bool MpegDashHeader::process_movie_box(utils::BinaryReader &moov) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(moov.data(), chunks);

        const auto mvhd_itr = chunks.find(MAKEFOURCC('m', 'v', 'h', 'd'));
        if(mvhd_itr == chunks.end()) {
            log->error("No MVHD chunk found in MOOV data");
            return false;
        }

        const auto header = (MovieHeaderBox*) mvhd_itr->second.data.data().data();
        log->info("MP4 creation time: {}", utils::time_to_iso_string(header->creation_time));

        return true;
    }
}