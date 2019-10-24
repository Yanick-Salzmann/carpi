#include "common_utils/network.hpp"

#include <sys/socket.h>
#include <vector>
#include <common_utils/log.hpp>
#include <common_utils/error.hpp>

namespace carpi::utils {
    Logger log{"network_utils"};

    void read(int socket, void *buffer, std::size_t size) {
        auto* cur_buffer = (uint8_t*) buffer;
        std::size_t num_read = 0;
        while(num_read < size) {
            const auto to_read = size - num_read;
            const auto read = recv(socket, cur_buffer, to_read, 0);
            if(read < 0) {
                log->error("Error reading from socket: {} (errno={})", error_to_string(errno), errno);
                throw std::runtime_error{"Error reading from socket"};
            }

            if(read == 0) {
                log->error("No more data available on socket");
                throw std::runtime_error{"Error reading from socket"};
            }

            cur_buffer += read;
            num_read += read;
        }
    }
}