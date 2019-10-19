#ifndef CARPI_OBD_INTAKEMAINFOLDPRESSURECODEC_HPP
#define CARPI_OBD_INTAKEMAINFOLDPRESSURECODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class IntakeManifoldPressureCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 1;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            return utils::Any(static_cast<uint32_t>(data[0]));
        }
    };
}

#endif //CARPI_OBD_INTAKEMAINFOLDPRESSURECODEC_HPP
