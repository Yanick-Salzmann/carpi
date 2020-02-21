#ifndef CARPI_NET_UTILS_UDP_BROADCAST_SERVER_HPP
#define CARPI_NET_UTILS_UDP_BROADCAST_SERVER_HPP

#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <common_utils/log.hpp>
#include <vector>

namespace carpi::net {
    class udp_broadcast {
        LOGGER;

        int _socket = 0;

        sa_family_t _address_family{};
        sockaddr_in _ip4_addr{};
        sockaddr_in6 _ip6_addr{};

        void convert_to_receiver();

    public:
        explicit udp_broadcast(uint16_t port, bool receiver);

        std::ptrdiff_t send_data(const std::vector<uint8_t>& data);
        std::ptrdiff_t read_data(std::vector<uint8_t>& data);
        std::ptrdiff_t read_data(void* buffer, std::size_t to_read);
    };
}

#endif //CARPI_NET_UTILS_UDP_BROADCAST_SERVER_HPP
