#include <common_utils/string.hpp>
#include "bluetooth_broadcast.hpp"

namespace carpi::gps {
    LOGGER_IMPL(BluetoothBroadcast);

    BluetoothBroadcast::BluetoothBroadcast(const std::string &mode, std::string target_device) {
        const auto lower_mode = utils::to_lower(mode);
        if(lower_mode == "client") {
            init_client_mode();
        } else {
            if(lower_mode != "server") {
                log->warn("Invalid bluetooth broadcast mode '{}'. Must be 'client' or 'server'. Defaulting to 'server'", lower_mode);
            }

            init_server_mode();
        }
    }

    void BluetoothBroadcast::init_server_mode() {
        log->info("Initializing passive bluetooth broadcast");
    }

    void BluetoothBroadcast::init_client_mode() {
        log->info("Initializing active bluetooth broadcast");
    }
}