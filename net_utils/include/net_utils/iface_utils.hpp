#ifndef CARPI_NET_UTILS_IFACE_UTILS_HPP
#define CARPI_NET_UTILS_IFACE_UTILS_HPP

#include <netinet/in.h>

namespace carpi::net {
    bool get_broadcast_address(sa_family_t& type, sockaddr_in& ip4_bcast, sockaddr_in6& ip6_bcast);
}

#endif //CARPI_NET_UTILS_IFACE_UTILS_HPP
