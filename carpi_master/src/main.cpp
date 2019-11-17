#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/comm_server.hpp"
#include <ui/web_core.hpp>
#include <data_server/http_server.hpp>
#include <io/camera/camera_stream.hpp>
#include <mqueue.h>

namespace carpi {
    int _argc;
    char **_argv;

    void mq_listener() {
        mq_attr attributes{
                .mq_maxmsg = 5,
                .mq_msgsize = 512
        };
        const auto mq = mq_open("/gpsbroadcast", O_RDWR | O_CREAT | O_CLOEXEC, 0777, &attributes);
        std::array<std::uint8_t, 512> message_data{};
        const auto msg_size = mq_receive(mq, (char *) message_data.data(),512, nullptr);
        std::cout << "Read: " << msg_size << std::endl;
    }

    int main(int argc, char *argv[]) {
        std::thread t{mq_listener};
        std::string line{};
        utils::Logger log{"main"};
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