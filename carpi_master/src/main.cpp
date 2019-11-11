#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/CommServer.hpp"
#include <ui/WebCore.hpp>
#include <data_server/http_server.hpp>
#include <io/camera/camera_stream.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>

namespace carpi {
    int _argc;
    char **_argv;

    int main(int argc, char *argv[]) {
        std::string line{};
        utils::Logger log{"main"};
        _argc = argc;
        _argv = argv;

        bluetooth::BluetoothManager mgr{};
        const auto devices = mgr.scan_devices(4);
        for(const auto& dev : devices) {
            log->info("Device: {} / {}", dev.address_string(), dev.device_name());
        }

        sCameraStream->init_shared_memory();

        video::H264Conversion::initialize_ffmpeg();

        data::HttpServer http_server{8081};

        ui::WebCore core{};

        CommServer server{};

        log->info("Press ENTER to shut down application");
        std::cin.sync();
        std::getline(std::cin, line);

        core.manual_shutdown();

        server.shutdown_acceptor();
        http_server.shutdown();

        return 0;
    }
}

int main(int argc, char *argv[]) {
    return carpi::main(argc, argv);
}