#include <common_utils/conversion.hpp>
#include "obd/protocols/AbstractCANProtocol.hpp"

namespace carpi::obd::protocols {
    LOGGER_IMPL(AbstractCANProtocol);

    AbstractCANProtocol::AbstractCANProtocol(const StringVector &init_lines, std::size_t num_id_bits) : _num_id_bits(num_id_bits) {
        process_init_lines(init_lines);
    }

    bool AbstractCANProtocol::parse_frame(msg::ObdFrame &frame) {
        auto raw = frame.raw();
        if (_num_id_bits == 11) {
            raw = std::string{"00000"} + raw;
        }

        std::vector<uint8_t> raw_data{};
        if (!parse_and_verify_frame_data(raw, raw_data, 6, 12)) {
            return false;
        }

        if (_num_id_bits == 11) {
            frame.priority(raw_data[2] & 0x0Fu)
                    .addr_mode(raw_data[3] & 0xF0u);

            if (frame.addr_mode() == 0xD0) {
                frame.rx_id(raw_data[3] & 0x0Fu)
                        .tx_id(0xF1);
            } else if (raw_data[3] & 0x08u) {
                frame.rx_id(0xF1)
                        .tx_id(raw_data[3] & 0x07u);
            } else {
                frame.rx_id(raw_data[3] & 0x07u)
                        .tx_id(0xF1);
            }
        } else {
            frame.priority(raw_data[0])
                    .addr_mode(raw_data[1])
                    .rx_id(raw_data[2])
                    .tx_id(raw_data[3]);
        }

        std::vector<uint8_t> frame_data{raw_data.begin() + 4, raw_data.end()};
        frame.data(frame_data)
                .type((frame_data[0] & 0xF0u) >> 4u);

        switch(frame.type()) {
            case FRAME_TYPE_SINGLE: {
                frame.data_size(frame_data[0] & 0x0Fu);
                if(!frame.data_size()) {
                    return false;
                }
                break;
            }

            case FRAME_TYPE_FIRST: {
                frame.data_size(((frame_data[0] & 0x0Fu) << 8u) | (frame_data[1]));
                if(!frame.data_size()) {
                    return false;
                }

                break;
            }

            case FRAME_TYPE_SEQUENCE: {
                frame.sequence(frame_data[0] & 0x0Fu);
                break;
            }

            default: {
                log->debug("Dropping frame with unknown frame type. Frame data: {}", frame);
                return false;
            }
        }

        return true;
    }

    bool AbstractCANProtocol::tx_id_engine_constant(uint32_t &tx_id) const {
        tx_id = TX_ID_ENGINE;
        return true;
    }

    bool AbstractCANProtocol::tx_id_transmission_constant(uint32_t &tx_id) const {
        tx_id = TX_ID_TRANSMISSION;
        return true;
    }

    bool AbstractCANProtocol::parse_message(msg::ObdMessage &msg) {
        auto frames = msg.frames();
        if(frames.size() == 1) {
            const auto& frame = frames[0];
            if(frame.type() != FRAME_TYPE_SINGLE) {
                log->warn("Dropping message with single frame that is not marked as FRAME_TYPE_SINGLE: {}", frame);
                return false;
            }

            auto frame_data = frame.data();
            msg.data() = std::vector<uint8_t>{frame_data.begin() + 1, frame_data.begin() + 1 + frame.data_size()};
        } else {
            std::vector<msg::ObdFrame> first_frames{};
            std::vector<msg::ObdFrame> next_frames{};

            for(const auto& frame : msg.frames()) {
                if(frame.type() == FRAME_TYPE_FIRST) {
                    first_frames.emplace_back(frame);
                } else if(frame.type() == FRAME_TYPE_SEQUENCE) {
                    next_frames.emplace_back(frame);
                } else {
                    log->debug("Ignoring frame that is not marked as FRAME_TYPE_FIRST or FRAME_TYPE_SEQUENCE in multi frame message: {}", frame);
                }
            }

            if(first_frames.size() != 1) {
                log->warn("Received multiple or no frames marked as FRAME_TYPE_FIRST. Must be exactly one, but got {}. Dropping message", first_frames.size());
                return false;
            }

            if(next_frames.empty()) {
                log->warn("Received multiple frames, but none marked as FRAME_TYPE_SEQUENCE. Dropping message");
                return false;
            }

            auto last_seq = next_frames[0].sequence();
            for(auto i = std::size_t{1}; i < next_frames.size(); ++i) {
                auto& cur_frame = next_frames[i];
                auto seq = (last_seq & 0xF0u) + cur_frame.sequence();
                if(seq + 7 < last_seq) {
                    seq += 0x10;
                }

                last_seq = seq;
                cur_frame.sequence(seq);
            }

            std::sort(next_frames.begin(), next_frames.end(), [](const auto& f1, const auto& f2) {
                return f1.sequence() < f2.sequence();
            });

            if(!verify_frame_order(next_frames)) {
                return false;
            }

            std::vector<uint8_t> msg_data{};
            const auto first_frame_data = first_frames[0].data();
            msg_data.insert(msg_data.end(), first_frame_data.begin() + 2, first_frame_data.end());
            for(const auto& frame : next_frames) {
                const auto frame_data = frame.data();
                msg_data.insert(msg_data.end(), frame_data.begin() + 1, frame_data.end());
            }

            msg_data.resize(first_frames[0].data_size());
            msg.data() = msg_data;
        }

        if(msg.data()[0] == 0x43) {
            const auto dtc_size = msg.data()[1] * 2;
            msg.data().resize(dtc_size + 2);
        }

        return true;
    }

    bool AbstractCANProtocol::verify_frame_order(const std::vector<msg::ObdFrame> &frames) {
        uint32_t next_index = 1;
        for (const auto &frame : frames) {
            if (frame.sequence() != next_index) {
                return false;
            }

            ++next_index;
        }

        return true;
    }
}