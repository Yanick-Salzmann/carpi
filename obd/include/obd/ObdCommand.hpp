#ifndef CARPI_OBD_ICOMMAND_HPP
#define CARPI_OBD_ICOMMAND_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include "EcuType.hpp"
#include "response/ResponseConverter.hpp"

namespace carpi::obd {
    class ObdCommand {
        uint32_t _service_id;
        std::string _pid;
        EcuType _ecu_type;
        std::shared_ptr<response::IResponseConverter> _response_converter;

    public:
        ObdCommand(
                uint32_t service,
                std::string pid,
                EcuType ecu_type,
                std::shared_ptr<response::IResponseConverter> response_converter
        );

        [[nodiscard]] uint32_t service_id() const {
            return _service_id;
        }

        [[nodiscard]] std::string pid() const {
            return _pid;
        }

        [[nodiscard]] EcuType ecu_type() const {
            return _ecu_type;
        }
    };
}

#endif //CARPI_OBD_ICOMMAND_HPP
