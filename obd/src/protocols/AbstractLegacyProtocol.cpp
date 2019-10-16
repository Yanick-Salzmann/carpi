#include "obd/protocols/AbstractLegacyProtocol.hpp"
#include "common_utils/byte_stream.hpp"
#include "common_utils/conversion.hpp"

namespace carpi::obd::protocols {
    LOGGER_IMPL(AbstractLegacyProtocol);

    AbstractLegacyProtocol::AbstractLegacyProtocol(const StringVector &init_lines) {
        process_init_lines(init_lines);
    }

    bool AbstractLegacyProtocol::parse_frame(msg::ObdFrame &frame) {
        const auto raw = frame.raw();
        if (raw.size() % 2) {
            log->debug("Received odd sized OBD frame which will be ignored: {}", frame);
            return false;
        }

        utils::BinaryStream stream{utils::hex2bytes(raw)};
        if(stream.size() < 6) {
            log->debug("Received frame that is too small. Frame {} has size {}, needs to be at least 6", frame, stream.size());
            return false;
        }

        if(stream.size() > 11) {
            log->debug("Received frame that is too long. Frame {} has size {}, needs to be less than 11", frame, stream.size());
            return false;
        }

        return true;
    }
}