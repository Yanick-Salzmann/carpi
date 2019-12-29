#include <mutex>
#include "net_utils/ssl_socket.hpp"
#include <openssl/err.h>
#include <sstream>
#include <sys/socket.h>
#include <netdb.h>
#include <common_utils/error.hpp>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

namespace carpi::net {
    LOGGER_IMPL(SslSocket);

    static std::once_flag _ssl_socket_once_flag{};

    SslSocket::SslSocket() {
        std::call_once(_ssl_socket_once_flag, [=]() {
            SSL_library_init();
            SSLeay_add_ssl_algorithms();
            SSL_load_error_strings();
        });

        const auto methods = TLS_client_method();
        _ssl_ctx = SSL_CTX_new(methods);
        _ssl = SSL_new(_ssl_ctx);
        if (!_ssl) {
            print_ssl_errors();
            throw std::runtime_error{"Error initializing SSL context"};
        }
    }

    SslSocket::~SslSocket() {
        if (_ssl != nullptr) {
            SSL_free(_ssl);
        }

        if (_ssl_ctx != nullptr) {
            SSL_CTX_free(_ssl_ctx);
        }

        shutdown();
    }

    void SslSocket::print_ssl_errors() {
        std::stringstream stream;
        stream << "Error executing OpenSSL function:" << std::endl;

        std::size_t error_code;
        while ((error_code = ERR_get_error()) != 0) {
            const auto str = ERR_error_string(error_code, nullptr);

            if (str != nullptr) {
                stream << str << std::endl;
            } else {
                stream << error_code << std::endl;
            }
        }

        log->error(stream.str());
    }

    void SslSocket::connect(const std::string &host, uint16_t port) {
        if (_is_connected) {
            log->warn("Socket is already connected");
            throw std::runtime_error{"Cannot connect socket multiple times"};
        }

        addrinfo addr_hints{};
        addr_hints.ai_family = AF_UNSPEC;
        addr_hints.ai_socktype = SOCK_STREAM;
        addr_hints.ai_protocol = IPPROTO_TCP;

        std::string port_str = std::to_string(port);

        addrinfo *addresses = nullptr;
        const auto error = getaddrinfo(host.c_str(), port_str.c_str(), &addr_hints, &addresses);
        if (error != 0) {
            log->warn("Error resolving host name '{}': {} ({})", host, gai_strerror(error), error);
            throw std::runtime_error{"Error resolving host name"};
        }

        std::shared_ptr<addrinfo> addr_ptr{addresses, [](addrinfo *addrs) { freeaddrinfo(addrs); }};

        auto last_error = 0;
        for (auto addr = addresses; addr != nullptr; addr = addr->ai_next) {
            _socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if (_socket == -1) {
                last_error = errno;
                continue;
            }

            if (::connect(_socket, addr->ai_addr, addr->ai_addrlen) == 0) {
                memcpy(_remote_addr, addr->ai_addr, addr->ai_addrlen);
                last_error = 0;
                break;
            }

            last_error = errno;
            close(_socket);
            _socket = -1;
        }

        if (last_error != 0) {
            log->warn("Error finding a suitable socket connection, last error was: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error{"Cannot connect to any address"};
        }

        const auto ssl_sock = SSL_get_fd(_ssl);
        SSL_set_fd(_ssl, _socket);
        const auto ssl_err = SSL_connect(_ssl);
        if (ssl_err <= 0) {
            print_ssl_errors();
            throw std::runtime_error{"Error connecting SSL socket"};
        }
    }

    void SslSocket::write(const void *buffer, std::size_t num_bytes) {
        const auto rc = SSL_write(_ssl, buffer, static_cast<int32_t>(num_bytes));
        if (rc <= 0) {
            print_ssl_errors();
            throw std::runtime_error{"Error writing to SSL Socket"};
        }
    }

    std::string SslSocket::to_string() {
        sockaddr_in6* addr = (sockaddr_in6*) _remote_addr;


        std::stringstream stream;
        stream << "<SslSocket remote=";
        char addr_buffer[50]{};
        const char* addr_str = nullptr;
        if (addr->sin6_family == AF_INET) {
            const auto *sin = (sockaddr_in *) _remote_addr;
            addr_str = inet_ntop(sin->sin_family, &sin->sin_addr, addr_buffer, sizeof addr_buffer);
        } else {
            addr_str = inet_ntop(addr->sin6_family, &addr->sin6_addr, addr_buffer, sizeof addr_buffer);
        }

        if(addr_str) {
            stream << addr_str;
        } else {
            stream << "null";
        }

        stream << " cipher=";
        const auto cipher_ptr = SSL_get_cipher_name(_ssl);
        if(cipher_ptr != nullptr) {
            stream << cipher_ptr;
        } else {
            stream << "unknown";
        }
        stream << ">";
        return stream.str();
    }

    uint8_t SslSocket::read_one() {
        uint8_t ret{};
        const auto rc = SSL_read(_ssl, &ret, sizeof ret);
        if(rc <= 0) {
            print_ssl_errors();
            throw std::runtime_error{"Error reading from Socket"};
        }

        return ret;
    }

    void SslSocket::read_all(void *buffer, std::size_t num_bytes) {
        auto cur_ptr = (uint8_t*) buffer;
        std::size_t to_read = num_bytes;
        while(to_read) {
            const auto rc = SSL_read(_ssl, cur_ptr, static_cast<int>(to_read));
            if(rc <= 0) {
                print_ssl_errors();
                throw std::runtime_error{"Error reading from Socket"};
            }

            cur_ptr += rc;
            to_read -= rc;
        }
    }

    void SslSocket::shutdown() {
        if(_socket < 0) {
            return;
        }

        close(_socket);
        _socket = -1;
    }
}