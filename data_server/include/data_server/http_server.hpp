#ifndef CARPI_DATA_SERVER_HTTP_SERVER_HPP
#define CARPI_DATA_SERVER_HTTP_SERVER_HPP

#include <cstdint>
#include <common_utils/log.hpp>

#include <thread>
#include <memory>
#include <list>

namespace carpi::data {
    class HttpConnection;

    class HttpServer {
        LOGGER;

        int32_t _server_socket = 0;

        std::thread _acceptor_thread;
        volatile bool _is_running = true;

        std::list<std::shared_ptr<HttpConnection>> _active_connections{};

        void acceptor_loop();

        void initialize_socket(uint16_t port);
    public:
        explicit HttpServer(uint16_t port);
        ~HttpServer();

        void shutdown();
    };
}

#endif //CARPI_DATA_SERVER_HTTP_SERVER_HPP
