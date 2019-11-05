#include "common_utils/process.hpp"
#include "common_utils/error.hpp"
#include "common_utils/log.hpp"

#include <unistd.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

namespace carpi::utils {
    utils::Logger logger{"carpi::utils::Subprocess"}; // NOLINT(cert-err58-cpp)

    SubProcess launch_subprocess(const std::string &command, char **arguments) {
        SubProcess ret_process{};

        int32_t stdin_pipe[2];
        if(pipe(stdin_pipe) < 0) {
            logger->error("Error creating stdin pipe for subprocess: {} (errno={})", error_to_string(errno), errno);
            return SubProcess{.error_code = errno};
        }

        int32_t stdout_pipe[2];
        if(pipe(stdout_pipe) < 0) {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);

            logger->error("Error creating stdout pipe for subprocess: {} (errno={})", error_to_string(errno), errno);
            return SubProcess{.error_code = errno};
        }

        int32_t stderr_pipe[2];
        if(pipe(stderr_pipe) < 0) {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);

            logger->error("Error creating stderr pipe for subprocess: {} (errno={})", error_to_string(errno), errno);
            return SubProcess{.error_code = errno};
        }

        const auto child = fork();
        if(child == 0) {
            if (dup2(stdin_pipe[PIPE_READ], STDIN_FILENO) == -1) {
                exit(errno);
            }

            if (dup2(stdout_pipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
                exit(errno);
            }

            if (dup2(stderr_pipe[PIPE_WRITE], STDERR_FILENO) == -1) {
                exit(errno);
            }

            const auto result = execv(command.c_str(), arguments);
            exit(result);
        } else if(child > 0) {
            close(stdin_pipe[PIPE_READ]);
            close(stdout_pipe[PIPE_WRITE]);
            close(stderr_pipe[PIPE_WRITE]);

            ret_process.stdin_pipe = stdin_pipe[PIPE_WRITE];
            ret_process.stdout_pipe = stdout_pipe[PIPE_READ];
            ret_process.stderr_pipe = stderr_pipe[PIPE_READ];

            ret_process.process_id = child;
        } else {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);
            close(stderr_pipe[0]);
            close(stderr_pipe[1]);

            logger->error("Error forking process: {} (errno={})", error_to_string(errno), errno);
            return SubProcess{.error_code = errno};
        }

        return ret_process;
    }
}