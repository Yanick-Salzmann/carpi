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

        const auto raw_data = utils::hex2bytes(raw);
        if (raw_data.size() < 6) {
            log->debug("Received frame that is too small. Frame {} has size {}, needs to be at least 6", frame, raw_data.size());
            return false;
        }

        if (raw_data.size() > 11) {
            log->debug("Received frame that is too long. Frame {} has size {}, needs to be less than 11", frame, raw_data.size());
            return false;
        }

        std::vector<uint8_t> frame_data{};
        frame_data.assign(raw_data.begin() + 3, raw_data.end() - 1);

        frame.data(frame_data)
                .priority(raw_data[0])
                .rx_id(raw_data[1])
                .tx_id(raw_data[2]);

        return true;
    }
}