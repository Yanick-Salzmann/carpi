#ifndef CARPI_OBD_OBDCOMMANDLIST_HPP
#define CARPI_OBD_OBDCOMMANDLIST_HPP

#include <map>
#include "ObdCommand.hpp"

namespace carpi::obd {
    extern std::map<std::string, ObdCommand> COMMAND_LIST;

    void initialize_commands();
};

#endif //CARPI_OBD_OBDCOMMANDLIST_HPP
