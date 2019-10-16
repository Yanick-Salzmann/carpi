#include "obd/protocols/AbstractOBDProtocol.hpp"

#include <algorithm>

namespace carpi::obd::protocols {
    std::vector<msg::ObdMessage> AbstractOBDProtocol::parse_messages(const StringVector &lines) {
        StringVector obd_lines{};
        StringVector invalid_lines{};

        partition_lines(lines, obd_lines, invalid_lines);

        const auto frames = parse_frames(obd_lines);

        std::map<uint32_t, std::vector<msg::ObdFrame>> frames_by_tx_id{};
        for(const auto& frame : frames) {
            frames_by_tx_id[frame.tx_id()].emplace_back(frame);
        }

        std::vector<msg::ObdMessage> messages{};
        messages.reserve(frames_by_tx_id.size());

        for(const auto& ecu_pair : frames_by_tx_id) {
            msg::ObdMessage msg{ecu_pair.second};
            const auto ecu_type_itr = _ecu_map.find(ecu_pair.first);

            if(ecu_type_itr != _ecu_map.end()) {
                msg.ecu_type(ecu_type_itr->second);
            } else {
                msg.ecu_type(EcuType::UNKNOWN);
            }

            messages.emplace_back(msg);
        }

        return messages;
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

    void AbstractOBDProtocol::load_ecu_mapping_from_init_messages(const std::vector<msg::ObdMessage> &init_messages) {
        if(init_messages.size() == 1) {
            _ecu_map[init_messages[0].tx_id()] = EcuType::ENGINE;
            return;
        }

        uint32_t tx_id_engine, tx_id_transmission;
        const auto has_tx_id_engine = tx_id_engine_constant(tx_id_engine);
        const auto has_tx_id_transmission = tx_id_transmission_constant(tx_id_transmission);

        auto has_engine = false;

        for(const auto& msg : init_messages) {
            if(has_tx_id_engine && msg.tx_id() == tx_id_engine) {
                _ecu_map[msg.tx_id()] = EcuType::ENGINE;
                has_engine = true;
            } else if(has_tx_id_transmission && msg.tx_id() == tx_id_transmission) {
                _ecu_map[msg.tx_id()] = EcuType::TRANSMISSION;
            } else {
                _ecu_map[msg.tx_id()] = EcuType::UNKNOWN;
            }
        }
    }

    void AbstractOBDProtocol::process_init_lines(const StringVector &init_lines) {
        const auto init_messages = parse_messages(init_lines);
        load_ecu_mapping_from_init_messages(init_messages);
    }
}