#ifndef CARPI_OBD_OBDMESSAGE_HPP
#define CARPI_OBD_OBDMESSAGE_HPP

#include <string>
#include <vector>

#include "ObdFrame.hpp"

namespace carpi::obd::msg {
    class ObdMessage {
        std::vector<ObdFrame> _frames;

    public:
        explicit ObdMessage(std::vector<ObdFrame> frames) : _frames(std::move(frames)) {}

        static bool is_hex_only(const std::string& message);
    };
}

#endif //CARPI_OBDMESSAGE_HPP
