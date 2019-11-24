#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/comm_server.hpp"
#include <ui/web_core.hpp>
#include <data_server/http_server.hpp>
#include <io/camera/camera_stream.hpp>
#include <wiring_utils/gpio.hpp>

namespace carpi {
    int _argc;
    char **_argv;

    int main(int argc, char *argv[]) {
        utils::Logger log{"main"};

        wiring::Gpio gpio;
        auto pin = gpio.open_pin(40);
        while(true) {
            pin.high();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            pin.low();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::string line{};
        _argc = argc;
        _argv = argv;

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