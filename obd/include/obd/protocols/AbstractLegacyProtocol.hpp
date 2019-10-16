#ifndef CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP

#include "common_utils/log.hpp"

#include "AbstractOBDProtocol.hpp"

namespace carpi::obd::protocols {
    class AbstractLegacyProtocol : public AbstractOBDProtocol {
        LOGGER;

        bool _has_tx_id_engine = false;
        bool _has_tx_id_transmission = false;

        uint32_t _tx_id_engine = 0;
        uint32_t _tx_id_transmission = 0;

    protected:
        explicit AbstractLegacyProtocol(const StringVector& init_lines);
        AbstractLegacyProtocol(const StringVector& init_lines, uint32_t tx_id_engine);
        AbstractLegacyProtocol(const StringVector& init_lines, uint32_t tx_id_engine, uint32_t tx_id_transmission);

        bool parse_frame(msg::ObdFrame &frame) override;
        bool parse_message(msg::ObdMessage& msg) override;

        bool tx_id_engine_constant(uint32_t &tx_id) const override;

        bool tx_id_transmission_constant(uint32_t &tx_id) const override;
    };
}

#endif //CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
