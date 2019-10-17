#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

#include <obd/ObdInterface.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        video::H264Conversion::initialize_ffmpeg();
        CommServer server{};
        bluetooth::BluetoothManager btmgr{};

        utils::Logger log{"main"};

        std::string line{};
        std::shared_ptr<bluetooth::BluetoothConnection> selected_device = nullptr;

        do {
            std::set<bluetooth::BluetoothDevice> devices = btmgr.scan_devices(5);
            uint32_t index = 1;
            for(auto& device : devices) {
                log->info("{}) {}", index++, device);
            }

            std::getline(std::cin, line);
            uint32_t device_index = 0;
            std::stringstream parse_stream;
            parse_stream << line;
            if(parse_stream >> device_index) {
                if(device_index < devices.size()) {
                    auto device_itr = devices.begin();
                    auto dev_index = 0u;
                    while(dev_index < device_index) {
                        ++device_itr;
                        ++dev_index;
                    }

                    selected_device = device_itr->connect(0x01);
                    break;
                }
            }
        } while(line != "q");

        if(line != "q") {
            log->info("Selected a device");
            obd::ObdInterface obd_iface{selected_device};
            while(true) {
                std::string command{};
                if(!std::getline(std::cin, command)) {
                    break;
                }

                if(command[0] == 'q') {
                    break;
                }

                obd_iface.dispatch_raw_command(command);
            }
        }

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}