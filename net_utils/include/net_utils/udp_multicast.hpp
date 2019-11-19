#ifndef CARPI_NET_UTILS_UDP_MULTICAST_HPP
#define CARPI_NET_UTILS_UDP_MULTICAST_HPP

#include <cstdint>
#include <string>
#include <common_utils/log.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace carpi::net {
    class UdpMulticast {
        LOGGER;

        int32_t _socket = 0;

        sa_family_t  _addr_family;
        sockaddr_in _addr4{};
        sockaddr_in6 _addr6{};

        void bind_receiver(const in_addr& gaddr4, const in6_addr& gaddr6);

    public:
        explicit UdpMulticast(const std::string& address, uint16_t port, bool receiver = false);

        std::ptrdiff_t send_data(const std::vector<uint8_t>& data);
        std::ptrdiff_t read_data(std::vector<uint8_t>& data);
        std::ptrdiff_t read_data(void* buffer, std::size_t to_read);
    };
}

#endif //CARPI_NET_UTILS_UDP_MULTICAST_HPP
