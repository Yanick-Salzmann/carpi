#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/comm_server.hpp"
#include <ui/web_core.hpp>
#include <data_server/http_server.hpp>
#include <io/camera/camera_stream.hpp>
#include <net_utils/udp_multicast.hpp>
#include <gps/gps_listener.hpp>
#include <gps/gps_constants.hpp>

namespace carpi {
    int _argc;
    char **_argv;

    gps::GpsMeasurement read_measurement(net::UdpMulticast &bcast) {
        static uint8_t buffer[32]{};
        const auto read = bcast.read_data(buffer, sizeof buffer);
        std::cout << "READ: {}" << read << std::endl;
        return {
                .lat = *((double*) buffer + 8),
                .lon = *((double*) buffer + 16),
                .alt = *((double*) buffer + 24)
        };
    }

    int main(int argc, char *argv[]) {
        utils::Logger log{"main"};
        net::UdpMulticast bcast{gps::gps_multicast_interface(), 3377, true};
        while (true) {
            gps::GpsMeasurement m = read_measurement(bcast);
            //log->info("GPS: {}/{}/{}", m.lat, m.lon, m.alt);
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