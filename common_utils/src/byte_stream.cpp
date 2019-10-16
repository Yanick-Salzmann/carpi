#include "common_utils/byte_stream.hpp"

#include <stdexcept>

namespace carpi::utils {
    LOGGER_IMPL(BinaryStream);

    BinaryStream &BinaryStream::read(void *buffer, std::size_t num_bytes) {
        if(_position + num_bytes > _data.size()) {
            log->warn("Attempted to read past the end of the stream: {} + {} > {}", _position, num_bytes, _data.size());
            throw std::runtime_error("Attempted to read past the end of the stream");
        }

        memcpy(buffer, _data.data() + _position, num_bytes);
        _position += num_bytes;

        return *this;
    }
}