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
        return false;
    }

    bool AbstractLegacyProtocol::tx_id_engine_constant(uint32_t &tx_id) const {
        if(!_has_tx_id_engine) {
            return false;
        }

        tx_id = _tx_id_engine;
        return true;
    }

    bool AbstractLegacyProtocol::tx_id_transmission_constant(uint32_t &tx_id) const {
        if(!_has_tx_id_transmission) {
            return false;
        }

        tx_id = _tx_id_transmission;
        return true;
    }
}