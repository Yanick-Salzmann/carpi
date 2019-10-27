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

class MemoryStreamSource : public carpi::video::IStreamSource {
    LOGGER;

    std::vector<uint8_t> _data;
    std::size_t _position = 0;

public:
    explicit MemoryStreamSource(std::vector<uint8_t> data) : _data(std::move(data)) { }

    size_t read(void *buffer, std::size_t num_bytes) override {
        if(_position >= _data.size()) {
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
    char** _argv;

    int main(int argc, char* argv[]) {
        _argc = argc;
        _argv = argv;

        //ui::WebCore core{};

        video::H264Conversion::initialize_ffmpeg();

        std::ifstream is{"camera.h264", std::ios::binary};
        is.seekg(0, std::ios::end);
        const auto data_size = is.tellg();
        is.seekg(0, std::ios::beg);
        std::vector<uint8_t> data(data_size);
        is.read((char*) data.data(), data_size);
        const auto stream_source = std::make_shared<MemoryStreamSource>(data);
        const auto stream = std::make_shared<video::H264Stream>(stream_source, 1920, 1080, 30);
        utils::Logger log{"main"};

        std::ofstream os{"camera.webm", std::ios::binary};

        video::H264Conversion conversion{stream, "webm", [&os](void* data, std::size_t size) { os.write((const char*) data, size); }, [&os]() { os.close(); log->info("DONE"); }};

        CommServer server{};

        log->info("Press ENTER to shut down application");
        std::cin.sync();
        std::string line{};
        std::getline(std::cin, line);

        //core.manual_shutdown();

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}