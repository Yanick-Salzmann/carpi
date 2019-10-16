#ifndef CARPI_OBD_ICOMMAND_HPP
#define CARPI_OBD_ICOMMAND_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace carpi::obd::commands {
    class ObdCommand {
        uint32_t _service_id;
        std::string _pid;

    public:
        ObdCommand(uint32_t service, std::string pid);

        [[nodiscard]] uint32_t service_id() const {
            return _service_id;
        }

        [[nodiscard]] std::string pid() const {
            return _pid;
        }
    };
}

#endif //CARPI_OBD_ICOMMAND_HPP
