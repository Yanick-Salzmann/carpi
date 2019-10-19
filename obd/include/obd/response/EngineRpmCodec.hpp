#ifndef CARPI_OBD_ENGINERPMCODEC_HPP
#define CARPI_OBD_ENGINERPMCODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class EngineRpmCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 2;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            const auto base = static_cast<uint32_t>(data[0]) * 256 + static_cast<uint32_t>(data[1]);
            return utils::Any(static_cast<float>(base) / 4.0f);
        }
    };
}

#endif //CARPI_OBD_ENGINERPMCODEC_HPP
