#ifndef CARPI_ABSTRACTCANPROTOCOL_HPP
#define CARPI_ABSTRACTCANPROTOCOL_HPP

#include <cstdint>

namespace carpi::obd::protocols {
    class AbstractCANProtocol {
        std::size_t _num_id_bits = 11;

    protected:
        explicit AbstractCANProtocol(std::size_t num_id_bits);
    };
}

#endif //CARPI_ABSTRACTCANPROTOCOL_HPP
