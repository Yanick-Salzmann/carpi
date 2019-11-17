#include "net_broadcast.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <common_utils/error.hpp>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

namespace carpi::gps {
    LOGGER_IMPL(NetBroadcast);

    NetBroadcast::NetBroadcast() {
        _socket = socket(AF_INET, SOCK_DGRAM, 0);

        uint32_t broadcast = 1;

        if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) < 0) {
            log->error("Error setting SO_BROADCAST on socket: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting socket to broadcast"};
        }

        ifaddrs *if_addrs = nullptr;
        if (getifaddrs(&if_addrs) < 0) {
            log->error("Error getting interface addresses: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting interface address"};
        }

        std::shared_ptr<ifaddrs> if_addr_ptr(if_addrs, [](ifaddrs *addrs) {
            freeifaddrs(addrs);
        });

        while (if_addrs != nullptr) {
            if ((if_addrs->ifa_flags & IFF_BROADCAST) != 0
                && (if_addrs->ifa_flags & IFF_LOOPBACK) == 0
                && (if_addrs->ifa_flags & IFF_RUNNING) != 0
                && if_addrs->ifa_ifu.ifu_broadaddr != nullptr && (if_addrs->ifa_ifu.ifu_broadaddr->sa_family == AF_INET || if_addrs->ifa_ifu.ifu_broadaddr->sa_family == AF_INET6)) {
                break;
            }

            if_addrs = if_addrs->ifa_next;
        }

        char ipv6_addr[1024]{};
        if (inet_ntop(if_addrs->ifa_ifu.ifu_broadaddr->sa_family,
                      if_addrs->ifa_ifu.ifu_broadaddr->sa_family == AF_INET ? (void *) (&((sockaddr_in *) if_addrs->ifa_ifu.ifu_broadaddr)->sin_addr) : (&((sockaddr_in6 *) if_addrs->ifa_ifu.ifu_broadaddr)->sin6_addr),
                      ipv6_addr,
                      sizeof ipv6_addr) == nullptr) {
            log->error("inet_ntop: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Cannot setup net broadcast"};
        }

        log->info("Selecting broadcast address: {} (interface: {}, flags: {})", ipv6_addr, if_addrs->ifa_name, if_addrs->ifa_flags);
        _address_family = if_addrs->ifa_addr->sa_family;
        if (_address_family == AF_INET) {
            memcpy(&_ip4_broadcast, if_addrs->ifa_ifu.ifu_broadaddr, sizeof _ip4_broadcast);
            _ip4_broadcast.sin_port = htons(3377);
        } else {
            memcpy(&_ip6_broadcast, if_addrs->ifa_ifu.ifu_broadaddr, sizeof _ip6_broadcast);
            _ip6_broadcast.sin6_port = htons(3377);
        }
    }

    void NetBroadcast::send_packet(const ipc::IpcPackage &packet) {
        // int16_t is intentional, doesnt hurt to be smaller
        if (packet.size() >= std::numeric_limits<int16_t>::max()) {
            log->error("Packet is too large: {} >= {}", packet.size(), std::numeric_limits<int16_t>::max());
            throw std::runtime_error{"Packet too large"};
        }

        std::vector<uint8_t> data{};
        packet.to_buffer(data);

        const auto rc = sendto(_socket,
               data.data(),
               data.size(),
               0,
               _address_family == AF_INET ? (sockaddr *) &_ip4_broadcast : (sockaddr *) &_ip6_broadcast,
               _address_family == AF_INET ? sizeof _ip4_broadcast : sizeof _ip6_broadcast
        );

        if(rc < 0) {
            log->error("Error sending data: {} (errno={})", utils::error_to_string(errno), errno);
        }
    }

    void NetBroadcast::on_measurement(const GpsMeasurement &gps_data) {
        log->info("GPS: {}/{}/{}", gps_data.lat, gps_data.lon, gps_data.alt);
        ipc::IpcPackage package{ipc::Opcodes::MSG_GPS_UPDATE};
        package << gps_data.lat << gps_data.lon << gps_data.alt;
        send_packet(package);
    }
}