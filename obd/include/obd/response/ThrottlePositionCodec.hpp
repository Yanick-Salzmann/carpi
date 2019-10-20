#ifndef CARPI_OBD_THROTTLEPOSITIONCODEC_HPP
#define CARPI_OBD_THROTTLEPOSITIONCODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class ThrottlePositionCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 1;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            return utils::Any(static_cast<float>(data[0]) / 2.55f);
        }
    };
}

#endif //CARPI_OBD_THROTTLEPOSITIONCODEC_HPP
