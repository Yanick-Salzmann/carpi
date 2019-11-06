#include "common_utils/process.hpp"
#include "common_utils/error.hpp"
#include "common_utils/log.hpp"

#include <unistd.h>
#include <iostream>

#define PIPE_READ 0
#define PIPE_WRITE 1

namespace carpi::utils {
    utils::Logger logger{"carpi::utils::Subprocess"}; // NOLINT(cert-err58-cpp)

    SubProcess launch_subprocess(const std::string &command, std::vector<std::string> arguments) {
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
            close(stdin_pipe[PIPE_WRITE]);
            close(stdout_pipe[PIPE_READ]);
            close(stderr_pipe[PIPE_READ]);

            if (dup2(stdin_pipe[PIPE_READ], STDIN_FILENO) == -1) {
                exit(errno);
            }

            if (dup2(stdout_pipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
                exit(errno);
            }

            if (dup2(stderr_pipe[PIPE_WRITE], STDERR_FILENO) == -1) {
                exit(errno);
            }

            close(stdin_pipe[PIPE_READ]);
            close(stdout_pipe[PIPE_WRITE]);
            close(stderr_pipe[PIPE_WRITE]);

            char** proc_args = !arguments.empty() ? new char*[arguments.size() + 1] : nullptr;
            if(!arguments.empty()) {
                for(auto i = std::size_t{0}; i < arguments.size(); ++i) {
                    proc_args[i] = strdup(arguments[i].c_str());
                }

                proc_args[arguments.size()] = nullptr;
            }

            execvp(command.c_str(), proc_args);
            exit(errno);
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