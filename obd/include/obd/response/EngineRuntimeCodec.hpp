#ifndef CARPI_OBD_ENGINERUNTIMECODEC_HPP
#define CARPI_OBD_ENGINERUNTIMECODEC_HPP

#include "ResponseConverter.hpp"
#include <iostream>

namespace carpi::obd::response {
    class EngineRuntimeCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 2;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            const auto num_seconds = static_cast<uint32_t>(data[0]) * 256 + static_cast<uint32_t>(data[1]);
            return utils::Any(num_seconds);
        }
    };
}

#endif //CARPI_OBD_ENGINERUNTIMECODEC_HPP
