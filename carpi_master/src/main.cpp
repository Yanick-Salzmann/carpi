#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include "comm/CommServer.hpp"

namespace carpi {
    int main(int argc, char* argv[]) {
        video::H264Conversion::initialize_ffmpeg();
        CommServer server{};
        std::string line{};
        do {
            std::getline(std::cin, line);
        } while(line != "q");

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}