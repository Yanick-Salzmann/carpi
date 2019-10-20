#ifndef CARPI_OBD_TIMINGADVANCECODEC_HPP
#define CARPI_OBD_TIMINGADVANCECODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class TimingAdvanceCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 1;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            return utils::Any(static_cast<float>(data[0]) / 2.0f - 64.0f);
        }
    };
}

#endif //CARPI_OBD_TIMINGADVANCECODEC_HPP
