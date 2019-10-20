#ifndef CARPI_OBD_AIRFLOWRATECODEC_HPP
#define CARPI_OBD_AIRFLOWRATECODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class AirflowRateCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 2;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            const auto a = static_cast<uint32_t>(data[0]);
            const auto b = static_cast<uint32_t>(data[1]);

            return utils::Any((256.0f * a + b) / 100.0f);
        }
    };
}

#endif //CARPI_OBD_AIRFLOWRATECODEC_HPP
