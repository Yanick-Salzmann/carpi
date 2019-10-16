#ifndef CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP

#include "common_utils/log.hpp"

#include "AbstractOBDProtocol.hpp"

namespace carpi::obd::protocols {
    class AbstractLegacyProtocol : public AbstractOBDProtocol {
        LOGGER;

    protected:
        explicit AbstractLegacyProtocol(const StringVector& init_lines);

        bool parse_frame(msg::ObdFrame &frame) override;
        bool parse_message(msg::ObdMessage& msg) override;
    };
}

#endif //CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
