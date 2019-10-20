#ifndef CARPI_OBD_OXYGENSENSORCODEC_HPP
#define CARPI_OBD_OXYGENSENSORCODEC_HPP

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class OxygenSensorResult {
        friend std::ostream& operator << (std::ostream&, const OxygenSensorResult&);

        float _voltage = 0.0f;
        float _fuel_trim = 0.0f;

    public:
        OxygenSensorResult(uint8_t a, uint8_t b) {
            _voltage = static_cast<float>(a) / 200.0f;
            if(b != 0xFF) {
                _fuel_trim = static_cast<float>(b) / 1.28f - 100.0f;
            } else {
                _fuel_trim = std::numeric_limits<float>::quiet_NaN();
            }
        }
    };

    std::ostream& operator << (std::ostream& stream, const OxygenSensorResult& sensor_result);

    class OxygenSensorCodec : public IResponseConverter {
    public:
        [[nodiscard]] size_t response_size() const override {
            return 2;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            return utils::Any(OxygenSensorResult{data[0], data[1]});
        }
    };
}

#endif //CARPI_OBD_OXYGENSENSORCODEC_HPP
