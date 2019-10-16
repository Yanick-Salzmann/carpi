#include "obd/protocols/AbstractLegacyProtocol.hpp"

namespace carpi::obd::protocols {

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines) {
        process_init_lines(init_lines);
    }

    bool AbstractLegacyProtocol::parse_frame(msg::ObdFrame &frame) {
        return false;
    }
}