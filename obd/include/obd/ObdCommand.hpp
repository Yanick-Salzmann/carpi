#ifndef CARPI_OBD_ICOMMAND_HPP
#define CARPI_OBD_ICOMMAND_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace carpi::obd::commands {
    class ObdCommand {
        std::string _pid;

    public:
        explicit ObdCommand(std::string pid);

        void write_to_buffer(std::vector<uint8_t>& buffer);
    };
}

#endif //CARPI_OBD_ICOMMAND_HPP
