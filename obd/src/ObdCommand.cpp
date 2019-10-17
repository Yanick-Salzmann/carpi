#include "obd/ObdCommand.hpp"

namespace carpi::obd {

    ObdCommand::ObdCommand(uint32_t service, std::string pid, EcuType ecu_type) :
            _service_id(service),
            _pid(std::move(pid)),
            _ecu_type(ecu_type) {

    }
}