#include <common_utils/string.hpp>
#include <common_utils/time.hpp>
#include "mpeg_dash.hpp"

namespace carpi::spotify::media {
#define MAKEFOURCC(ch0, ch1, ch2, ch3)  \
    ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8u) |  \
    ((uint32_t)(uint8_t)(ch2) << 16u) | ((uint32_t)(uint8_t)(ch3) << 24u ))

    LOGGER_IMPL(MpegDashHeader);
    LOGGER_IMPL(MpegDashSegment);

    std::string fourcc_to_string(uint32_t value) {
        const auto *ptr = (const char *) &value;
        return std::string{ptr, ptr + 4};
    }

    void fourcc_reader(utils::BinaryReader &reader, std::multimap<uint32_t, MpegDashChunk> &chunk_map) {
        while (reader.available() >= 8) {
            const auto size = reader.read<uint32_t, utils::big_endian_policy>();
            const auto signature = reader.read<uint32_t>();

            MpegDashChunk chunk{
                    .fourcc = signature
            };


            if (size > 8) {
                std::vector<uint8_t> chunk_data(size - 8);
                reader.read(chunk_data);
                chunk.data = utils::BinaryReader{chunk_data};
            }

            chunk_map.emplace(signature, chunk);
        }
    }

    void fourcc_reader(const std::vector<uint8_t> &data, std::multimap<uint32_t, MpegDashChunk> &chunk_map) {
        utils::BinaryReader reader{data};
        fourcc_reader(reader, chunk_map);
    }

    MpegDashHeader::MpegDashHeader(const std::vector<uint8_t> &data) {
        fourcc_reader(data, _chunks);
        if (!verify_format()) {
            log->error("Unknown MP4 format");
            return;
        }

        if (!verify_movie_box()) {
            log->error("MP4 metadata cannot be parsed");
            return;
        }
    }

    bool MpegDashHeader::verify_format() {
        const auto itr = _chunks.find(MAKEFOURCC('f', 't', 'y', 'p'));
        if (itr == _chunks.end()) {
            log->info("Chunk 'ftyp' not found");
            return false;
        }

        const auto major_version = itr->second.data.read<uint32_t>();
        const auto minor_version = itr->second.data.read<uint32_t>();

        if (major_version != MAKEFOURCC('i', 's', 'o', 'm') && major_version != MAKEFOURCC('d', 'a', 's', 'h')) {
            log->warn("Major version of MP4 file is not 'isom' or 'dash', this is not supported (fourcc={:x})", major_version);
            return false;
        }

        const auto num_types = itr->second.data.available() / 4;

        for (auto i = 0; i < num_types; ++i) {
            const auto brand = fourcc_to_string(itr->second.data.read<uint32_t>());
            log->info("Supported brand for MP4 file: {}", brand);
        }

        return true;
    }

    bool MpegDashHeader::verify_movie_box() {
        auto itr = _chunks.find(MAKEFOURCC('m', 'o', 'o', 'v'));
        if (itr == _chunks.end()) {
            log->error("No MOOV chunk found in MP4 header");
            return false;
        }

        return process_movie_box(itr->second.data);
    }

    bool MpegDashHeader::process_movie_box(utils::BinaryReader &moov) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(moov.data(), chunks);

        const auto mvhd_itr = chunks.find(MAKEFOURCC('m', 'v', 'h', 'd'));
        if (mvhd_itr == chunks.end()) {
            log->error("No MVHD chunk found in MOOV data");
            return false;
        }

        auto header = *(MovieHeaderBox *) mvhd_itr->second.data.data().data();
        header.to_big_endian();
        log->info("MP4 creation time: {}", utils::time_to_iso_string(header.utc_creation_time()));
        log->info("MP4 modification time: {}", utils::time_to_iso_string(header.utc_modification_time()));
        _time_scale = header.time_scale;

        const auto trak_itr = chunks.find(MAKEFOURCC('t', 'r', 'a', 'k'));
        if (trak_itr == chunks.end()) {
            log->error("No TRAK chunk found in MOOV data");
            return false;
        }

        return process_track(trak_itr->second.data);
    }

    bool MpegDashHeader::process_track(utils::BinaryReader &trak) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(trak.data(), chunks);

        auto mdia_itr = chunks.find(MAKEFOURCC('m', 'd', 'i', 'a'));
        if (mdia_itr == chunks.end()) {
            log->error("Missing MDIA chunk in MOOV data");
            return false;
        }

        return process_media_info(mdia_itr->second.data);
    }

    bool MpegDashHeader::process_media_info(utils::BinaryReader &mdia) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(mdia.data(), chunks);

        auto minf_itr = chunks.find(MAKEFOURCC('m', 'i', 'n', 'f'));
        if (minf_itr == chunks.end()) {
            log->error("Missing MINF chunks in MDIA data");
            return false;
        }

        return process_media_metadata(minf_itr->second.data);
    }

    bool MpegDashHeader::process_media_metadata(utils::BinaryReader &minf) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(minf.data(), chunks);

        const auto stbl_itr = chunks.find(MAKEFOURCC('s', 't', 'b', 'l'));
        if (stbl_itr == chunks.end()) {
            log->error("Missing STBL chunk in MINF data");
            return false;
        }

        return process_sample_table(stbl_itr->second.data);
    }

    bool MpegDashHeader::process_sample_table(utils::BinaryReader &stbl) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(stbl.data(), chunks);

        const auto stsd_itr = chunks.find(MAKEFOURCC('s', 't', 's', 'd'));
        if (stsd_itr == chunks.end()) {
            log->error("Missing STSD chunk in STBL data");
            return false;
        }

        return process_sample_description(stsd_itr->second.data);
    }

    bool MpegDashHeader::process_sample_description(utils::BinaryReader &stsd) {
        stsd.seek_mod(8); // version + flags
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(stsd, chunks);

        const auto enca_itr = chunks.find(MAKEFOURCC('e', 'n', 'c', 'a'));
        if (enca_itr == chunks.end()) {
            log->error("Missing ENCA chunk in STSD data");
            return false;
        }

        return process_encryption_box(enca_itr->second.data);
    }

    bool MpegDashHeader::process_encryption_box(utils::BinaryReader &enca) {
        enca.seek_mod(28);
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(enca, chunks);

        const auto sinf_itr = chunks.find(MAKEFOURCC('s', 'i', 'n', 'f'));

        if (sinf_itr == chunks.end()) {
            log->error("Missing SINF chunk in ENCA data");
            return false;
        }

        return process_scheme_info(sinf_itr->second.data);
    }

    bool MpegDashHeader::process_scheme_info(utils::BinaryReader &sinf) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(sinf, chunks);

        const auto schm_itr = chunks.find(MAKEFOURCC('s', 'c', 'h', 'm'));
        const auto schi_itr = chunks.find(MAKEFOURCC('s', 'c', 'h', 'i'));

        if (schm_itr == chunks.end()) {
            log->error("Missing SCHM chunk in SINF data");
            return false;
        }

        auto &schm_reader = schm_itr->second.data;
        schm_reader.seek_mod(4);

        if (schm_reader.read<uint32_t>() != MAKEFOURCC('c', 'e', 'n', 'c')) {
            log->error("Encryption scheme type must be 'cenc'.");
            return false;
        }

        if (schi_itr == chunks.end()) {
            log->error("Missing SCHI chunk in SINF data");
            return false;
        }


        return process_protection_info(schi_itr->second.data);
    }

    bool MpegDashHeader::process_protection_info(utils::BinaryReader &schi) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(schi, chunks);

        const auto tenc_itr = chunks.find(MAKEFOURCC('t', 'e', 'n', 'c'));
        if (tenc_itr == chunks.end()) {
            log->error("Missing TENC chunk in SINF data");
            return false;
        }

        tenc_itr->second.data.seek_mod(4); // flags always 0

        std::vector<uint8_t> encrypted(3);
        tenc_itr->second.data.read(encrypted);
        for (auto i = 0; i < 3; ++i) {
            if (encrypted[i] != 0) {
                _is_encrypted = true;
                break;
            }
        }

        _iv_size = tenc_itr->second.data.read<uint8_t>();
        tenc_itr->second.data.read(&_key_id, sizeof _key_id);
        return true;
    }

    MpegDashSegment::MpegDashSegment(MpegDashHeader &header, const std::vector<uint8_t> &data) : _header{header} {
        fourcc_reader(data, _chunks);

        const auto moof_itr = _chunks.find(MAKEFOURCC('m', 'o', 'o', 'f'));
        if (moof_itr == _chunks.end()) {
            log->error("Missing MOOF chunk in segment");
            return;
        }

        _movie_header = moof_itr->second.data.data();

        if (!process_movie_header(moof_itr->second.data)) {
            return;
        }

        const auto mdat_itr = _chunks.find(MAKEFOURCC('m', 'd', 'a', 't'));
        if(mdat_itr == _chunks.end()) {
            log->error("No MODAT chunk in segment");
            return;
        }

        _movie_data = mdat_itr->second.data.data();

        auto min_data_size = 0; //_data_offset;
        for(const auto& sample : _sample_sizes) {
            min_data_size += sample;
        }
    }

    bool MpegDashSegment::process_movie_header(utils::BinaryReader &moof) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(moof, chunks);

        const auto traf_itr = chunks.find(MAKEFOURCC('t', 'r', 'a', 'f'));
        if (traf_itr == chunks.end()) {
            log->error("Missing TRAF chunk in movie header");
            return false;
        }

        return process_track_fragment(traf_itr->second.data);
    }

    bool MpegDashSegment::process_track_fragment(utils::BinaryReader &traf) {
        std::multimap<uint32_t, MpegDashChunk> chunks{};
        fourcc_reader(traf, chunks);

        const auto senc_itr = chunks.find(MAKEFOURCC('s', 'e', 'n', 'c'));
        if (senc_itr == chunks.end()) {
            _is_encrypted = false;
            return true;
        }

        _is_encrypted = true;

        if (!process_sample_encryption(senc_itr->second.data)) {
            return false;
        }

        const auto trun_pair = chunks.equal_range(MAKEFOURCC('t', 'r', 'u', 'n'));
        if(trun_pair.first  == trun_pair.second) {
            log->error("No track run (TRUN) found in track fragment (TRAF)");
            return false;
        }

        if(std::distance(trun_pair.first, trun_pair.second) > 1) {
            log->warn("Multiple track runs (TRUN) found in track fragment (TRAF). This is not implemented yet");
            return false;
        }

        return process_track_run(trun_pair.first->second.data);
    }

    bool MpegDashSegment::process_sample_encryption(utils::BinaryReader &senc) {
        senc.seek_mod(4); // version + flags
        const auto sample_count = senc.read<uint32_t, utils::big_endian_policy>();
        _per_sample_iv.resize(sample_count);

        if (_header.iv_size() != 8 && _header.iv_size() != 16) {
            log->error("IV size {} not supported, must be 8 or 16 bytes", _header.iv_size());
            return false;
        }

        for (auto i = 0; i < sample_count; ++i) {
            senc.read(&_per_sample_iv[i], _header.iv_size());
        }

        return true;
    }

    bool MpegDashSegment::process_track_run(utils::BinaryReader &trun) {
        const auto flags = trun.read<uint32_t, utils::big_endian_policy>() & 0xFFFFFFu;
        if(flags != 0x201 && flags != 0x301) { // 0x200 -> sample size present, 0x100 -> sample duration present, 0x001 -> data offset present
            log->error("Track run does not have sample size and data offset present");
            return false;
        }

        const auto sample_count = trun.read<uint32_t, utils::big_endian_policy>();
        if(sample_count != _per_sample_iv.size()) {
            log->error("Track run does not have the same amount of data as sample encryption ({} vs {})", sample_count, _per_sample_iv.size());
            return false;
        }

        trun.seek_mod(4); // data_offset

        _sample_sizes.resize(sample_count);
        if(flags == 0x201) {
            trun.read(_sample_sizes);

            for(auto& sample : _sample_sizes) {
                sample = htonl(sample);
            }
        } else {
            for(auto i = 0; i < sample_count; ++i) {
                trun.seek_mod(4);
                _sample_sizes[i] = trun.read<uint32_t, utils::big_endian_policy>();
            }
        }

        return true;
    }

    void MpegDashSegment::write_movie_header(FILE* f) {
        uint32_t signature = MAKEFOURCC('m', 'o', 'o', 'f');
        uint32_t size = htonl(static_cast<uint32_t>(_movie_header.size()) + 8);
        fwrite(&size, 4, 1, f);
        fwrite(&signature, 4, 1, f);
        fwrite(_movie_header.data(), 1, _movie_header.size(), f);
    }

    void MpegDashSegment::write_movie_data(FILE* f, bool header_only) {
        uint32_t signature = MAKEFOURCC('m', 'd', 'a', 't');
        uint32_t size = htonl(static_cast<uint32_t>(_movie_data.size()) + 8);
        fwrite(&size, 4, 1, f);
        fwrite(&signature, 4, 1, f);
        if(!header_only) {
            fwrite(_movie_data.data(), 1, _movie_data.size(), f);
        }
    }
}