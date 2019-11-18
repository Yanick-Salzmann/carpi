#include "net_utils/iface_utils.hpp"
#include <common_utils/log.hpp>
#include <common_utils/error.hpp>
#include <memory>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>

namespace carpi::net {
    utils::Logger log{"carpi::net::iface_utils"};

    bool get_broadcast_address(sa_family_t &type, sockaddr_in &ip4_bcast, sockaddr_in6 &ip6_bcast) {
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
        type = if_addrs->ifa_addr->sa_family;
        if (type == AF_INET) {
            memcpy(&ip4_bcast, if_addrs->ifa_ifu.ifu_broadaddr, sizeof ip4_bcast);
        } else {
            memcpy(&ip6_bcast, if_addrs->ifa_ifu.ifu_broadaddr, sizeof ip6_bcast);
        }

        return true;
    }
}