#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

#include <obd/ObdInterface.hpp>
#include <obd/ObdCommandList.hpp>

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

                std::transform(command.begin(), command.end(), command.begin(), [](const auto& chr) { return std::toupper(chr); });
                const auto cmd_itr = obd::COMMAND_LIST.find(command);
                if(cmd_itr == obd::COMMAND_LIST.end()) {
                    log->warn("No command found: {}", command);
                    continue;
                }

                utils::Any response{};
                if(!obd_iface.send_command(cmd_itr->second, response)) {
                    log->warn("There was an error executing the command '{}'", command);
                    continue;
                }

                log->debug("Received: {}", response.to_string());
            }
        }

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}