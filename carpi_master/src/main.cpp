#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/comm_server.hpp"
#include <ui/web_core.hpp>
#include <data_server/http_server.hpp>
#include <io/camera/camera_stream.hpp>
#include <net_utils/udp_broadcast.hpp>
#include <gps/gps_listener.hpp>

namespace carpi {
    int _argc;
    char **_argv;

    template<typename T>
    T read_data(net::UdpBroadcast &bcast) {
        std::size_t to_read = sizeof(T);
        T ret;
        uint8_t *ptr = (uint8_t *) &ret;
        while (to_read > 0) {
            const auto num_read = bcast.read_data(ptr, to_read);
            if (num_read < 0) {
                throw std::runtime_error{"Error reading data"};
            }

            to_read -= num_read;
            ptr += num_read;
        }

        return ret;
    }

    gps::GpsMeasurement read_measurement(net::UdpBroadcast &bcast) {
        std::size_t size = read_data<std::size_t>(bcast);
        read_data<uint32_t>(bcast);
        return {
                .lat = read_data<double>(bcast),
                .lon = read_data<double>(bcast),
                .alt = read_data<double>(bcast)
        };
    }

    int main(int argc, char *argv[]) {
        utils::Logger log{"main"};
        net::UdpBroadcast bcast{3377, true};
        while (true) {
            gps::GpsMeasurement m = read_measurement(bcast);
            log->info("GPS: {}/{}/{}", m.lat, m.lon, m.alt);
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