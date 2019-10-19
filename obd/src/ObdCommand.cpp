#include "obd/ObdCommand.hpp"

namespace carpi::obd {

    ObdCommand::ObdCommand(uint32_t service, std::string pid, EcuType ecu_type, std::shared_ptr<response::IResponseConverter> response_converter) :
            _service_id(service),
            _pid(std::move(pid)),
            _ecu_type(ecu_type),
            _response_converter(std::move(response_converter)) {

    }
}