#include "obd/protocols/AbstractLegacyProtocol.hpp"
#include "common_utils/byte_stream.hpp"
#include "common_utils/conversion.hpp"

namespace carpi::obd::protocols {
    LOGGER_IMPL(AbstractLegacyProtocol);

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines) {
        process_init_lines(init_lines);
    }

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines, uint32_t tx_id_engine) :
            AbstractLegacyProtocol(init_lines) {
        _has_tx_id_engine = true;
        _tx_id_engine = tx_id_engine;
    }

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines, uint32_t tx_id_engine, uint32_t tx_id_transmission) :
            AbstractLegacyProtocol(init_lines, tx_id_engine) {
        _has_tx_id_transmission = true;
        _tx_id_transmission = tx_id_transmission;
    }

    bool AbstractLegacyProtocol::parse_frame(msg::ObdFrame &frame) {
        const auto raw = frame.raw();
        std::vector<uint8_t> raw_data{};
        if (!parse_and_verify_frame_data(raw, raw_data, 6, 11)) {
            return false;
        }

        std::vector<uint8_t> frame_data{};
        frame_data.assign(raw_data.begin() + 3, raw_data.end() - 1);

        frame.data(frame_data)
                .priority(raw_data[0])
                .rx_id(raw_data[1])
                .tx_id(raw_data[2]);

        return true;
    }

    bool AbstractLegacyProtocol::parse_message(msg::ObdMessage &msg) {
        auto frames = msg.frames();
        const auto mode = frames[0].data()[0];

        if (frames.size() > 1) {
            for (auto i = std::size_t{1}; i < frames.size(); ++i) {
                if (frames[i].data()[0] != mode) {
                    log->debug("Dropping message because frame {} has not the same mode as frame 0", i);
                    return false;
                }
            }
        }

        // GET_DTC
        if (mode == 0x43) {
            std::vector<uint8_t> msg_data{{0x43, 0x00}};
            for (const auto &frame : frames) {
                const auto frame_data = frame.data();
                msg_data.insert(msg_data.end(), frame_data.begin(), frame_data.end());
            }

            msg.data() = msg_data;
        } else {
            if (frames.size() == 1) {
                msg.data() = frames[0].data();
            } else {
                std::sort(frames.begin(), frames.end(), [](const msg::ObdFrame &f1, const msg::ObdFrame &f2) {
                    return f1.data()[2] < f2.data()[2];
                });

                if (!verify_frame_order(frames)) {
                    log->debug("Received out of sequence frames");
                    return false;
                }

                auto frame_data = frames[0].data();
                std::vector<uint8_t> msg_data{{frame_data[0], frame_data[1]}};
                msg_data.insert(msg_data.end(), frame_data.begin() + 3, frame_data.end());
                for(auto i = std::size_t{1}; i < frames.size(); ++i) {
                    frame_data = frames[i].data();
                    msg_data.insert(msg_data.end(), frame_data.begin() + 3, frame_data.end());
                }

                msg.data() = msg_data;
            }
        }

        return true;
    }

    bool AbstractLegacyProtocol::tx_id_engine_constant(uint32_t &tx_id) const {
        if (!_has_tx_id_engine) {
            return false;
        }

        tx_id = _tx_id_engine;
        return true;
    }

    bool AbstractLegacyProtocol::tx_id_transmission_constant(uint32_t &tx_id) const {
        if (!_has_tx_id_transmission) {
            return false;
        }

        tx_id = _tx_id_transmission;
        return true;
    }

    bool AbstractLegacyProtocol::verify_frame_order(const std::vector<msg::ObdFrame> &frames) {
        uint32_t next_index = 1;
        for (const auto &frame : frames) {
            if (frame.data()[2] != next_index) {
                return false;
            }

            ++next_index;
        }

        return true;
    }
}