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
        std::vector<uint8_t> raw_data{};
        if (!parse_and_verify_frame_data(raw, raw_data, 6, 11)) {
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

    bool AbstractLegacyProtocol::parse_message(msg::ObdMessage &msg) {
        return false;
    }
}