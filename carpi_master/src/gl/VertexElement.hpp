#ifndef CARPI_CARPI_VERTEXELEMENT_HPP
#define CARPI_CARPI_VERTEXELEMENT_HPP

#include <string>
#include <GLES2/gl2.h>

namespace carpi::gl {
    enum class DataType {
        BYTE = GL_UNSIGNED_BYTE,
        FLOAT = GL_FLOAT
    };

    class VertexElement {
        std::string _semantic{};
        uint32_t _index = 0;
        uint32_t _components = 0;
        DataType _data_type = DataType::FLOAT;
        bool _normalize = false;

    public:
        VertexElement(std::string semantic, uint32_t index, uint32_t components) :
                _semantic{std::move(semantic)},
                _index{index},
                _components{components} {

        }

        VertexElement(std::string semantic, uint32_t index, uint32_t components, DataType data_type, bool normalize) :
                _semantic{std::move(semantic)},
                _index{index},
                _components{components},
                _data_type{data_type},
                _normalize{normalize} {

        }
    };
}

#endif //CARPI_CARPI_VERTEXELEMENT_HPP
