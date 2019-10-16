#include "obd/msg/ObdMessage.hpp"

namespace carpi::obd::msg {

    bool ObdMessage::is_hex_only(const std::string &message) {
        for (const auto &chr : message) {
            if ((chr >= 'a' && chr <= 'f') || (chr >= 'A' && chr <= 'F') || (chr >= '0' && chr <= '9')) {
                continue;
            }

            return false;
        }

        return true;
    }
}