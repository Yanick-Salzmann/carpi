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
}