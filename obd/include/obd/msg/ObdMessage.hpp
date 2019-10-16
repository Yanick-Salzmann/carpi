#ifndef CARPI_OBD_OBDMESSAGE_HPP
#define CARPI_OBD_OBDMESSAGE_HPP

#include <string>

namespace carpi::obd::msg {
    class ObdMessage {
    public:
        static bool is_hex_only(const std::string& message);
    };
}

#endif //CARPI_OBDMESSAGE_HPP
