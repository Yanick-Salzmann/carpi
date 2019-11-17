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
        ifaddrs* if_addrs = nullptr;
        if(getifaddrs(&if_addrs) < 0) {
            log->error("Error getting interface addresses: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting interface address"};
        }

        std::shared_ptr<ifaddrs> if_addr_ptr(if_addrs, [](ifaddrs* addrs) {
            freeifaddrs(addrs);
        });

        std::string net_mask = inet_ntoa(((const sockaddr_in*) if_addrs->ifa_addr)->sin_addr);
        log->info("Net Mask: {}", net_mask);
    }
}