#ifndef CARPI_OBD_ABSTRACTCANPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTCANPROTOCOL_HPP

#include <cstdint>
#include "AbstractOBDProtocol.hpp"

#include <common_utils/log.hpp>

namespace carpi::obd::protocols {
    class AbstractCANProtocol : public AbstractOBDProtocol {
        static const uint32_t TX_ID_ENGINE = 0;
        static const uint32_t TX_ID_TRANSMISSION = 1;

        static const uint32_t FRAME_TYPE_SINGLE = 0;
        static const uint32_t FRAME_TYPE_FIRST = 1;
        static const uint32_t FRAME_TYPE_SEQUENCE = 2;

        LOGGER;

        std::size_t _num_id_bits = 11;

    protected:
        explicit AbstractCANProtocol(const StringVector& init_lines, std::size_t num_id_bits);

        bool tx_id_engine_constant(uint32_t &tx_id) const override;

        bool tx_id_transmission_constant(uint32_t &tx_id) const override;

        bool parse_frame(msg::ObdFrame &frame) override;
    };
}

#endif //CARPI_ABSTRACTCANPROTOCOL_HPP
