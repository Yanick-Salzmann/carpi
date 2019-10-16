#include "obd/protocols/AbstractOBDProtocol.hpp"

#include <algorithm>

namespace carpi::obd::protocols {

    AbstractOBDProtocol::AbstractOBDProtocol(const StringVector &init_lines) {
        const auto init_messages = parse_messages(init_lines);
    }

    std::vector<msg::ObdMessage> AbstractOBDProtocol::parse_messages(const StringVector &lines) {
        StringVector obd_lines{};
        StringVector invalid_lines{};

        partition_lines(lines, obd_lines, invalid_lines);

        const auto frames = parse_frames(obd_lines);

        return {};
    }

    void AbstractOBDProtocol::partition_lines(const StringVector &raw_lines, StringVector &obd_lines, StringVector &invalid_lines) {
        for(const auto& line : raw_lines) {
            std::string trimmed_line{};
            std::copy_if(line.begin(), line.end(), std::back_inserter(trimmed_line), [](const auto& chr) { return chr != ' '; });
            if(!msg::ObdMessage::is_hex_only(trimmed_line)) {
                invalid_lines.emplace_back(trimmed_line);
            } else {
                obd_lines.emplace_back(trimmed_line);
            }
        }
    }

    std::vector<msg::ObdFrame> AbstractOBDProtocol::parse_frames(const StringVector &obd_lines) {
        std::vector<msg::ObdFrame> frames{};
        for(const auto& line : obd_lines) {
            msg::ObdFrame frame{line};
            if(parse_frame(frame)) {
                frames.emplace_back(frame);
            }
        }

        return frames;
    }
}