#include "obd/response/EngineRuntimeCodec.hpp"

namespace carpi::obd::response {

    std::ostream& operator<<(std::ostream &stream, const std::chrono::seconds &seconds) {
        return stream << "<Duration seconds=" << seconds.count() << ">";
    }
}