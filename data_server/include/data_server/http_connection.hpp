#ifndef CARPI_DATA_SERVER_HTTP_CONNECTION_HPP
#define CARPI_DATA_SERVER_HTTP_CONNECTION_HPP

#include <cstdint>
#include <thread>

#include <common_utils/log.hpp>

namespace carpi::data {
    class HttpConnection {
        LOGGER;

        int32_t _socket;

        std::thread _request_thread;
        volatile bool _is_running = true;



        void request_loop();

        std::string read_until(const char& delimiter) {
            return read_until(std::string{delimiter});
        }

        std::string read_until(const std::string& delimiter);

    public:
        explicit HttpConnection(int32_t socket);

        void shutdown();
    };
}

#endif //CARPI_DATA_SERVER_HTTP_CONNECTION_HPP
