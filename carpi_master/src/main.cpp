#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

#include <obd/ObdInterface.hpp>
#include <obd/ObdCommandList.hpp>
#include <ui/WebCore.hpp>
#include <video_stream/H264Stream.hpp>

#include "main.hpp"
#include <fstream>

#include <wiringPi.h>

#include <data_server/http_server.hpp>

class MemoryStreamSource : public carpi::video::IStreamSource {
    LOGGER;

    std::vector<uint8_t> _data;
    std::size_t _position = 0;

public:
    explicit MemoryStreamSource(std::vector<uint8_t> data) : _data(std::move(data)) {}

    size_t read(void *buffer, std::size_t num_bytes) override {
        if (_position >= _data.size()) {
            return 0;
        }

        const auto to_read = std::min(num_bytes, _data.size() - _position);
        memcpy(buffer, &_data[_position], to_read);
        _position += to_read;
        return to_read;
    }
};

LOGGER_IMPL(MemoryStreamSource);

namespace carpi {
    int _argc;
    char **_argv;

    int main(int argc, char *argv[]) {
        _argc = argc;
        _argv = argv;

        video::H264Conversion::initialize_ffmpeg();
        utils::Logger log{"main"};

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