#include "obd/protocols/AbstractCANProtocol.hpp"

namespace carpi::obd::protocols {

    AbstractCANProtocol::AbstractCANProtocol(const std::vector<std::string> &init_lines, std::size_t num_id_bits) :
            AbstractOBDProtocol(init_lines),
            _num_id_bits(num_id_bits) {

    }
}