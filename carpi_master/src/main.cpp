#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include "comm/CommServer.hpp"

namespace carpi {
    int main(int argc, char* argv[]) {
        video::H264Conversion::initialize_ffmpeg();
        CommServer server{};
        bluetooth::BluetoothManager btmgr{};

        utils::Logger log{"main"};

        std::string line{};
        do {
            const auto devices = btmgr.scan_devices(5);
            uint32_t index = 1;
            for(const auto& device : devices) {
                log->info("{}) {}", index, device);
            }

            std::getline(std::cin, line);
        } while(line != "q");

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}