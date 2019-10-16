#ifndef CARPI_OBD_OBDFRAME_HPP
#define CARPI_OBD_OBDFRAME_HPP

#include <string>

namespace carpi::obd::msg {
    class ObdFrame {
        std::string _raw;
    };
}

#endif //CARPI_OBD_OBDFRAME_HPP
