#include "net_broadcast.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <common_utils/error.hpp>
#include <arpa/inet.h>

namespace carpi::gps {
    LOGGER_IMPL(NetBroadcast);

    NetBroadcast::NetBroadcast() {
        _socket = socket(AF_INET, SOCK_DGRAM, 0);
        ifaddrs *if_addrs = nullptr;
        if (getifaddrs(&if_addrs) < 0) {
            log->error("Error getting interface addresses: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting interface address"};
        }

        std::shared_ptr<ifaddrs> if_addr_ptr(if_addrs, [](ifaddrs *addrs) {
            freeifaddrs(addrs);
        });

        char ipv6_addr[1024]{};
        if (inet_ntop(if_addrs->ifa_addr->sa_family,
                      if_addrs->ifa_addr->sa_family == AF_INET ? (void *) (&((sockaddr_in *) if_addrs->ifa_addr)->sin_addr) : (&((sockaddr_in6 *) if_addrs->ifa_addr)->sin6_addr),
                      ipv6_addr,
                      sizeof ipv6_addr) == nullptr) {
            log->error("inet_ntop: {} (errno={})", utils::error_to_string(errno), errno);
        }
        log->info("Net Mask: {}", ipv6_addr);
    }
}