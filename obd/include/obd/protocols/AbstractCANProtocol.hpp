#ifndef CARPI_OBD_ABSTRACTCANPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTCANPROTOCOL_HPP

#include <cstdint>
#include "AbstractOBDProtocol.hpp"

namespace carpi::obd::protocols {
    class AbstractCANProtocol : public AbstractOBDProtocol {
        std::size_t _num_id_bits = 11;

    protected:
        explicit AbstractCANProtocol(const std::vector<std::string>& init_lines, std::size_t num_id_bits);
    };
}

#endif //CARPI_ABSTRACTCANPROTOCOL_HPP
