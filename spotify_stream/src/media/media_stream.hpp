#ifndef CARPI_MEDIA_STREAM_HPP
#define CARPI_MEDIA_STREAM_HPP

#include <fmod.hpp>
#include <cstdint>

namespace carpi::spotify::media {
    class MediaStream {
    public:
        virtual std::size_t read(void* buffer, std::size_t size) = 0;
        virtual std::size_t write(const void* buffer, std::size_t size) = 0;

        virtual bool read_supported() const = 0;
        virtual bool write_supported() const = 0;
    };
}

#endif //CARPI_MEDIA_STREAM_HPP
