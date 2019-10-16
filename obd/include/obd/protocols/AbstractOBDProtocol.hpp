#ifndef CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP

#include <vector>
#include <string>
#include <obd/msg/ObdMessage.hpp>
#include <obd/msg/ObdFrame.hpp>

namespace carpi::obd::protocols {
    typedef std::vector<std::string> StringVector;

    class AbstractOBDProtocol {
        static void partition_lines(const StringVector& raw_lines, StringVector& obd_lines, StringVector& invalid_lines);

        std::vector<msg::ObdFrame> parse_frames(const StringVector& obd_lines);

    protected:
        explicit AbstractOBDProtocol(const StringVector& init_lines);

        virtual bool parse_frame(msg::ObdFrame& frame) = 0;

        std::vector<msg::ObdMessage> parse_messages(const StringVector& lines);
    };
}

#endif //CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
