#include "Buffers.hpp"

namespace carpi::gl {

    Buffer::Buffer(GLenum buffer_type) : _buffer_type{buffer_type} {
        glGenBuffers(1, &_buffer);
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &_buffer);
    }

    Buffer &Buffer::bind() {
        glBindBuffer(_buffer_type, _buffer);
        return *this;
    }

    Buffer &Buffer::data(const void *data, std::size_t num_bytes) {
        bind();
        glBufferData(_buffer_type, num_bytes, data, GL_STATIC_DRAW);
        return *this;
    }
}