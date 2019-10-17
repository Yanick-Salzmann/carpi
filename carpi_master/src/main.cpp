#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

namespace carpi {
    int main(int argc, char* argv[]) {
        video::H264Conversion::initialize_ffmpeg();
        CommServer server{};
        bluetooth::BluetoothManager btmgr{};

        utils::Logger log{"main"};

        std::string line{};
        std::shared_ptr<bluetooth::BluetoothConnection> selected_device = nullptr;

        do {
            auto devices = btmgr.scan_devices(5);
            uint32_t index = 1;
            for(const auto& device : devices) {
                log->info("{}) {}", index++, device);
            }

            std::getline(std::cin, line);
            uint32_t device_index = 0;
            std::stringstream parse_stream;
            parse_stream << line;
            if(parse_stream >> device_index) {
                if(device_index < devices.size()) {
                    auto itr = devices.begin();
                    auto index = 0u;
                    while(index < device_index) {
                        ++itr;
                        ++index;
                    }

                    selected_device = (*itr).connect();
                }
            }
        } while(line != "q");

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}