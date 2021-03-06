#include <ipc_common/net_broadcast.hpp>
#include <common_utils/error.hpp>

namespace carpi::ipc {
    LOGGER_IMPL(NetBroadcast);

    NetBroadcast::NetBroadcast(std::string address, uint16_t port) {
        log->info("Starting UDP broadcast of GPS data on {}:{}", address, port);
        _broadcast = std::make_shared<net::upd_multicast>(address, port, false);
    }

    void NetBroadcast::send_packet(const ipc::IpcPackage &packet) {
        // int16_t is intentional, doesnt hurt to be smaller
        if (packet.size() >= std::numeric_limits<int16_t>::max()) {
            log->error("Packet is too large: {} >= {}", packet.size(), std::numeric_limits<int16_t>::max());
            throw std::runtime_error{"Packet too large"};
        }

        std::vector<uint8_t> data{};
        packet.to_buffer(data);

        const auto rc = _broadcast->send_data(data);
        if(rc < 0) {
            log->error("Error sending data: {} (errno={})", utils::error_to_string(errno), errno);
        }
    }
}