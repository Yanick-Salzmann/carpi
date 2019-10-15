#ifndef CARPI_OBD_RESPONSECONVERTER_HPP
#define CARPI_OBD_RESPONSECONVERTER_HPP

#include <cstdint>

namespace carpi::obd {
    class IResponseConverter {
    public:
        virtual ~IResponseConverter() = default;

        [[nodiscard]] virtual std::size_t response_size() const = 0;
    };
}

#endif //CARPI_OBD_RESPONSECONVERTER_HPP
