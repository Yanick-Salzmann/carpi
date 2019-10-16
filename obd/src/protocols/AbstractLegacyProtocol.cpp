#include "obd/protocols/AbstractLegacyProtocol.hpp"

namespace carpi::obd::protocols {

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines) : AbstractOBDProtocol(init_lines) {

    }
}