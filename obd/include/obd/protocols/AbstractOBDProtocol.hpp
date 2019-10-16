#ifndef CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP

#include <vector>
#include <string>

namespace carpi::obd::protocols {
    class AbstractOBDProtocol {
    protected:
        explicit AbstractOBDProtocol(const std::vector<std::string>& init_lines);
    };
}

#endif //CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
