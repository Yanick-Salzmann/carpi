#include "gps/gps_listener.hpp"

namespace carpi::gps {
    LOGGER_IMPL(GpsListener);

    GpsListener::GpsListener(const std::string &host, uint16_t port) {
        gps_data_t data{};
        const auto rc = gps_open(host.c_str(), std::to_string(port).c_str(), &data);
        log->info("gps_open = {}", rc);
    }
}