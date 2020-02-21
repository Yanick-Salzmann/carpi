#ifndef CARPI_NET_UTILS_SSL_SOCKET_HPP
#define CARPI_NET_UTILS_SSL_SOCKET_HPP

#include <openssl/ssl.h>
#include <common_utils/log.hpp>

namespace carpi::net {
    class ssl_socket {
        LOGGER;

        bool _is_connected = false;

        int _socket = -1;
        SSL* _ssl = nullptr;
        SSL_CTX* _ssl_ctx = nullptr;
        char _remote_addr[255];

        void print_ssl_errors();

    public:
        ssl_socket();
        ~ssl_socket();

        void connect(const std::string& host, uint16_t port);

        uint8_t read_one();
        void read_all(void* buffer, std::size_t num_bytes);

        void write(const void* buffer, std::size_t num_bytes);

        template<typename T>
        ssl_socket& write(T& value) {
            write(&value, sizeof value);
            return *this;
        }

        std::string to_string();

        void shutdown();
    };
}

#endif //CARPI_NET_UTILS_SSL_SOCKET_HPP
