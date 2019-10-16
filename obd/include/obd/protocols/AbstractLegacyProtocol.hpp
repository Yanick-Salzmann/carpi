#ifndef CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP

#include "AbstractOBDProtocol.hpp"

namespace carpi::obd::protocols {
    class AbstractLegacyProtocol : public AbstractOBDProtocol {
    protected:
        explicit AbstractLegacyProtocol(const StringVector& init_lines);
    };
}

#endif //CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
