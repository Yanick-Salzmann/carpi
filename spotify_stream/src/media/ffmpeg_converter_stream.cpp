#include <cstring>
#include "ffmpeg_converter_stream.hpp"

namespace carpi::spotify::media {

    std::size_t FfmpegConverterStream::read(void *buffer, std::size_t size) {
        std::unique_lock<std::mutex> l{_data_lock};
        if(_data.empty()) {
            _read_event.wait(l, [=]() { return !_data.empty() || _is_end_of_file; });
        }

        if(_is_end_of_file) {
            return 0;
        }

        const auto to_read = std::min<std::size_t>(size, _data.size());
        memcpy(buffer, _data.data(), to_read);
        _data.erase(_data.begin(), _data.begin() + to_read);
        return to_read;
    }

    std::size_t FfmpegConverterStream::write(const void *buffer, std::size_t size) {
        {
            std::lock_guard<std::mutex> l{_data_lock};
            _data.insert(_data.end(), (uint8_t*) buffer, ((uint8_t*) buffer) + size);
        }

        _read_event.notify_all();
        return size;
    }

    bool FfmpegConverterStream::read_supported() const {
        return true;
    }

    bool FfmpegConverterStream::write_supported() const {
        return true;
    }
}