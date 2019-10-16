#include "obd/protocols/AbstractCANProtocol.hpp"

namespace carpi::obd::protocols {

    AbstractCANProtocol::AbstractCANProtocol(std::size_t num_id_bits) : _num_id_bits(num_id_bits) {

    }
}