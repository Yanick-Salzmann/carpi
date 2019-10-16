#ifndef CARPI_OBD_OBDMESSAGE_HPP
#define CARPI_OBD_OBDMESSAGE_HPP

#include <string>
#include <vector>

#include "obd/msg/ObdFrame.hpp"
#include "obd/EcuType.hpp"

namespace carpi::obd::msg {
    class ObdMessage {
        std::vector<ObdFrame> _frames;
        EcuType _ecu_type = EcuType::UNKNOWN;

    public:
        explicit ObdMessage(std::vector<ObdFrame> frames) : _frames(std::move(frames)) {}

        [[nodiscard]] EcuType ecu_type() const {
            return _ecu_type;
        }

        ObdMessage& ecu_type(const EcuType& ecu_type) {
            _ecu_type = ecu_type;
            return *this;
        }

        [[nodiscard]] uint32_t tx_id() const {
            if(_frames.empty()) {
                return 0;
            }

            return _frames[0].tx_id();
        }

        static bool is_hex_only(const std::string& message);
    };
}

#endif //CARPI_OBDMESSAGE_HPP
