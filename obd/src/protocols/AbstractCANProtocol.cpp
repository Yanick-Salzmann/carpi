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

        return false;
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