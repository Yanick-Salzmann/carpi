#ifndef CARPI_OBD_RESPONSECONVERTER_HPP
#define CARPI_OBD_RESPONSECONVERTER_HPP

#include <cstdint>
#include <common_utils/any.hpp>
#include <vector>

namespace carpi::obd::response {
    class IResponseConverter {
    public:
        virtual ~IResponseConverter() = default;

        [[nodiscard]] virtual std::size_t response_size() const = 0;

        virtual utils::Any decode(const std::vector<uint8_t>& data) = 0;
    };
}

#endif //CARPI_OBD_RESPONSECONVERTER_HPP
