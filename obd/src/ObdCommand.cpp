#include "obd/ObdCommand.hpp"

namespace carpi::obd::commands {

    ObdCommand::ObdCommand(std::string pid) : _pid(std::move(pid)) {

    }
}