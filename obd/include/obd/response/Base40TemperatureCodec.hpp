#ifndef CARPI_OBD_BASE40TEMPERATORCODEC_HPP
#define CARPI_OBD_BASE40TEMPERATORCODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class Base40TemperatureCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 1;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            return utils::Any{static_cast<int32_t>(data[0]) - 40};
        }
    };
}

#endif //CARPI_OBD_BASE40TEMPERATORCODEC_HPP
