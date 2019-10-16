#include "obd/protocols/AbstractLegacyProtocol.hpp"

namespace carpi::obd::protocols {

    AbstractLegacyProtocol::AbstractLegacyProtocol(const std::vector<std::string> &init_lines) : AbstractOBDProtocol(init_lines) {

    }
}