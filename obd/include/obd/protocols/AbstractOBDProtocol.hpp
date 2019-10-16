#ifndef CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP

#include <vector>
#include <string>
#include <obd/msg/ObdMessage.hpp>

namespace carpi::obd::protocols {
    class AbstractOBDProtocol {
        typedef std::vector<std::string> StringVector;

        static void partition_lines(const StringVector& raw_lines, StringVector& obd_lines, StringVector& invalid_lines);

    protected:
        explicit AbstractOBDProtocol(const StringVector& init_lines);

        std::vector<msg::ObdMessage> parse_messages(const StringVector& lines);
    };
}

#endif //CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
