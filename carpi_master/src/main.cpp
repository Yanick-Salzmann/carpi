#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/CommServer.hpp"
#include <ui/WebCore.hpp>
#include <data_server/http_server.hpp>
#include <video_stream/RawCameraStream.hpp>

namespace carpi {
    int _argc;
    char **_argv;

    int main(int argc, char *argv[]) {
        utils::Logger log{"main"};
        _argc = argc;
        _argv = argv;

        video::H264Conversion::initialize_ffmpeg();

        FILE* of = fopen("output.yuv", "wb");

        video::RawCameraStream cam_stream{[of](auto data, auto size) {
            fwrite(data.data(), 1, size, of);
        }};

        cam_stream.initialize_camera({1920, 1080, 5, 30});

        std::cin.get();
        fflush(of);
        fclose(of);

        data::HttpServer http_server{8081};

        ui::WebCore core{};

        CommServer server{};

        log->info("Press ENTER to shut down application");
        std::cin.sync();
        std::string line{};
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