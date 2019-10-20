#ifndef CARPI_CARPI_BUFFERS_HPP
#define CARPI_CARPI_BUFFERS_HPP

#include <GLES2/gl2.h>
#include <cstdint>
#include <memory>

namespace carpi::gl {
    class Buffer {
        GLuint _buffer = 0;
        GLenum _buffer_type;

    protected:
        explicit Buffer(GLenum buffer_type);

    public:
        virtual ~Buffer();

        Buffer &bind();

        Buffer &data(const void *data, std::size_t num_bytes);
    };

    class VertexBuffer : public Buffer {
    public:
        VertexBuffer() : Buffer{GL_ARRAY_BUFFER} {}
    };

    enum class IndexType {
        uint8 = GL_UNSIGNED_BYTE,
        uint16 = GL_UNSIGNED_SHORT,
        uint32 = GL_UNSIGNED_INT
    };

    class IndexBuffer : public Buffer {
        IndexType _index_type = IndexType::uint8;
    public:
        IndexBuffer() : Buffer{GL_ELEMENT_ARRAY_BUFFER} {}

        explicit IndexBuffer(IndexType index_type) : Buffer{GL_ELEMENT_ARRAY_BUFFER}, _index_type{index_type} {}

        [[nodiscard]] IndexType index_type() const {
            return _index_type;
        }

        IndexBuffer& index_type(IndexType index_type) {
            _index_type = index_type;
            return *this;
        }
    };

    typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
    typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
}

#endif //CARPI_CARPI_BUFFERS_HPP
