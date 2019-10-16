#include "obd/ObdCommand.hpp"

namespace carpi::obd::commands {

    ObdCommand::ObdCommand(uint32_t service, std::string pid) : _service_id(service), _pid(std::move(pid)) {

    }
}