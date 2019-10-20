#include "obd/response/OxygenSensorCodec.hpp"

namespace carpi::obd::response {
    std::ostream &operator<<(std::ostream &stream, const OxygenSensorResult &sensor_result) {
        return stream << "<OxygenSensorResult voltage=" << sensor_result._voltage << ", fuel_trim=" << sensor_result._fuel_trim << ">";
    }
}
