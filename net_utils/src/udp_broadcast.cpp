#include "net_utils/udp_broadcast.hpp"
#include "net_utils/iface_utils.hpp"
#include <common_utils/error.hpp>
#include <arpa/inet.h>

namespace carpi::net {
    LOGGER_IMPL(UdpBroadcast);

    UdpBroadcast::UdpBroadcast(uint16_t port, bool receiver) {
        _socket = socket(AF_INET, SOCK_DGRAM, 0);

        uint32_t broadcast = 1;

        if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) < 0) {
            log->error("Error setting SO_BROADCAST on socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting socket to broadcast"};
        }

        if(!get_broadcast_address(_address_family, _ip4_addr, _ip6_addr)) {
            log->error("Error fetching broadcast address");
            throw std::runtime_error{"Error getting broadcast address"};
        }

        if(receiver) {
            _ip4_addr.sin_addr.s_addr = INADDR_ANY;
            _ip6_addr.sin6_addr = in6addr_any;
            convert_to_receiver();
        }

        _ip4_addr.sin_port = htons(port);
        _ip6_addr.sin6_port = htons(port);
    }

    std::ptrdiff_t UdpBroadcast::send_data(const std::vector<uint8_t> &data) {
        if(_address_family == AF_INET) {
            return sendto(_socket, data.data(), data.size(), 0, (const sockaddr*) &_ip4_addr, sizeof _ip4_addr);
        } else {
            return sendto(_socket, data.data(), data.size(), 0, (const sockaddr*) &_ip6_addr, sizeof _ip6_addr);
        }
    }

    std::ptrdiff_t UdpBroadcast::read_data(std::vector<uint8_t> &data) {
        return read_data(data.data(), data.size());
    }

    void UdpBroadcast::convert_to_receiver() {
        const sockaddr* addr_ptr = nullptr;
        socklen_t len = 0;
        if(_address_family == AF_INET) {
            addr_ptr = (const sockaddr*) &_ip4_addr;
            len = sizeof _ip4_addr;
        } else {
            addr_ptr = (const sockaddr*) &_ip6_addr;
            len = sizeof _ip6_addr;
        }

        if(!bind(_socket, addr_ptr, len)) {
            log->error("Error binding UDP broadcast receiver: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error binding UDP receiver"};
        }
    }

    std::ptrdiff_t UdpBroadcast::read_data(void *buffer, std::size_t to_read) {
        if(_address_family == AF_INET) {
            sockaddr_in remote_addr{};
            socklen_t remote_len = sizeof remote_addr;
            const auto num_read = recvfrom(_socket, buffer, to_read, 0, (sockaddr*)&remote_addr, &remote_len);
            if(num_read <= 0) {
                return num_read;
            }

            log->info("Received {} bytes from {}:{}", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
            return num_read;
        } else {
            sockaddr_in6 remote_addr{};
            socklen_t remote_len = sizeof remote_addr;
            const auto num_read = recvfrom(_socket,buffer, to_read, 0, (sockaddr*)&remote_addr, &remote_len);
            if(num_read <= 0) {
                return num_read;
            }

            char ip_buffer[100]{};
            log->info("Received {} bytes from {}:{}", inet_ntop(AF_INET6, &remote_addr.sin6_addr, ip_buffer, sizeof ip_buffer));
            return num_read;
        }
    }
}