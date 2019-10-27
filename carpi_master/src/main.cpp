#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

#include <obd/ObdInterface.hpp>
#include <obd/ObdCommandList.hpp>
#include <ui/WebCore.hpp>

#include "main.hpp"

namespace carpi {
    int _argc;
    char** _argv;

    int main(int argc, char* argv[]) {
        _argc = argc;
        _argv = argv;

        ui::WebCore core{};

        video::H264Conversion::initialize_ffmpeg();
        CommServer server{};

        utils::Logger log{"main"};
        log->info("Press ENTER to shut down application");
        std::cin.sync();
        std::string line{};
        std::getline(std::cin, line);

        core.manual_shutdown();

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}