#ifndef CARPI_COMMON_UTILS_BYTE_STREAM_HPP
#define CARPI_COMMON_UTILS_BYTE_STREAM_HPP

#include <cstdint>
#include <vector>

#include "log.hpp"

namespace carpi::utils {
    class BinaryStream {
        LOGGER;

        std::vector<uint8_t> _data;
        std::size_t _position = 0;

    public:
        explicit BinaryStream(std::vector<uint8_t> data) : _data(std::move(data)) { }
        BinaryStream() = default;

        [[nodiscard]] std::vector<uint8_t> data() const {
            return _data;
        }

        [[nodiscard]] std::size_t position() const {
            return _position;
        }

        [[nodiscard]] std::size_t size() const {
            return _data.size();

        }

        void position(std::size_t position) {
            _position = position;
        }

        BinaryStream& read(void* buffer, std::size_t num_bytes);
    };
}

#endif //CARPI_COMMON_UTILS_BYTE_STREAM_HPP
