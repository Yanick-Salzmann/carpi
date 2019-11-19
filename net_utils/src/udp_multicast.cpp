#include <sys/socket.h>
#include <common_utils/error.hpp>

#include "net_utils/udp_multicast.hpp"

namespace carpi::net {
    LOGGER_IMPL(UdpMulticast);

    UdpMulticast::UdpMulticast(const std::string &address, uint16_t port, bool receiver) {
        if (inet_pton(AF_INET6, address.c_str(), &_addr6) == 1) {
            _addr_family = AF_INET6;
        } else if (inet_pton(AF_INET, address.c_str(), &_addr4) == 1) {
            _addr_family = AF_INET;
        } else {
            log->error("Cannot parse '{}' as IPv4 or IPv6 address");
            throw std::runtime_error{"Invalid multicast address"};
        }

        _socket = socket(_addr_family, SOCK_DGRAM, 0);
        if (_socket < 0) {
            log->error("Error creating UDP socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating UDP socket"};
        }

        uint32_t reuse = 0;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse) < 0) {
            log->error("Error setting SO_REUSEADDR on UDP socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating UDP socket"};
        }

        _addr4.sin_port = htons(port);
        _addr6.sin6_port = htons(port);

        if (receiver) {
            const auto inaddr4 = _addr4.sin_addr;
            const auto inaddr6 = _addr6.sin6_addr;

            _addr4.sin_addr.s_addr = htonl(INADDR_ANY);
            _addr6.sin6_addr = in6addr_any;
            bind_receiver(inaddr4, inaddr6);
        }
    }

    void UdpMulticast::bind_receiver(const in_addr &gaddr4, const in6_addr &gaddr6) {
        if (_addr_family == AF_INET) {
            if (bind(_socket, (const sockaddr *) &_addr4, sizeof _addr4) < 0) {
                log->error("Error binding UDP socket: {} (errno={})", utils::error_to_string(errno), errno);
                throw std::runtime_error{"Error binding UDP"};
            }

            ip_mreq mreq{};
            mreq.imr_multiaddr = gaddr4;
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);

            if (setsockopt(_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0) {
                log->error("Error adding membership to multicast address: {} (errno={})", utils::error_to_string(errno), errno);
                throw std::runtime_error{"Error joining multicast"};
            }
        } else {
            if (bind(_socket, (const sockaddr *) &_addr6, sizeof _addr6) < 0) {
                log->error("Error binding UDP socket: {} (errno={})", utils::error_to_string(errno), errno);
                throw std::runtime_error{"Error binding UDP"};
            }

            ipv6_mreq mreq{
                    .ipv6mr_multiaddr = gaddr6,
                    .ipv6mr_interface = 0
            };

            if (setsockopt(_socket, IPPROTO_IPV6, IP_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0) {
                log->error("Error adding membership to multicast address: {} (errno={})", utils::error_to_string(errno), errno);
                throw std::runtime_error{"Error joining multicast"};
            }
        }
    }

    std::ptrdiff_t UdpMulticast::send_data(const std::vector<uint8_t> &data) {
        if (_addr_family == AF_INET) {
            return sendto(_socket, data.data(), data.size(), 0, (const sockaddr *) &_addr4, sizeof _addr4);
        } else {
            return sendto(_socket, data.data(), data.size(), 0, (const sockaddr *) &_addr6, sizeof _addr6);
        }
    }

    std::ptrdiff_t UdpMulticast::read_data(std::vector<uint8_t> &data) {
        return read_data(data.data(), data.size());
    }

    std::ptrdiff_t UdpMulticast::read_data(void *buffer, std::size_t to_read) {
        if (_addr_family == AF_INET) {
            sockaddr_in remote_addr{};
            socklen_t remote_len = sizeof remote_addr;
            return recvfrom(_socket, buffer, to_read, 0, (sockaddr *) &remote_addr, &remote_len);
        } else {
            sockaddr_in6 remote_addr{};
            socklen_t remote_len = sizeof remote_addr;
            return recvfrom(_socket, buffer, to_read, 0, (sockaddr *) &remote_addr, &remote_len);
        }
    }
}
