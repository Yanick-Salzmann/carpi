#ifndef CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP

#include "AbstractOBDProtocol.hpp"

namespace carpi::obd::protocols {
    class AbstractLegacyProtocol : public AbstractOBDProtocol {
    protected:
        explicit AbstractLegacyProtocol(const std::vector<std::string>& init_lines);
    };
}

#endif //CARPI_OBD_ABSTRACTLEGACYPROTOCOL_HPP
