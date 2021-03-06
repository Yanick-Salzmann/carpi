#ifndef CARPI_COMMON_UTILS_PROCESS_HPP
#define CARPI_COMMON_UTILS_PROCESS_HPP

#include <string>
#include <vector>

namespace carpi::utils {
    struct SubProcess {
        int stdin_pipe;
        int stdout_pipe;
        int stderr_pipe;

        int process_id;
        int error_code;
    };

    SubProcess launch_subprocess(const std::string& command, std::vector<std::string> arguments);
}

#endif //CARPI_COMMON_UTILS_PROCESS_HPP
