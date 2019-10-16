#ifndef CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP

#include <vector>
#include <string>
#include <map>

#include <obd/msg/ObdMessage.hpp>
#include <obd/msg/ObdFrame.hpp>
#include <obd/EcuType.hpp>

namespace carpi::obd::protocols {
    typedef std::vector<std::string> StringVector;

    class AbstractOBDProtocol {
        std::map<uint32_t, EcuType> _ecu_map;

        static void partition_lines(const StringVector& raw_lines, StringVector& obd_lines, StringVector& invalid_lines);

        std::vector<msg::ObdFrame> parse_frames(const StringVector& obd_lines);

        void load_ecu_mapping_from_init_messages(const std::vector<msg::ObdMessage>& init_messages);

    protected:
        AbstractOBDProtocol() = default;

        void process_init_lines(const StringVector& init_lines);

        virtual bool parse_frame(msg::ObdFrame& frame) = 0;
        virtual bool tx_id_engine_constant(uint32_t& tx_id) const { return false; };
        virtual bool tx_id_transmission_constant(uint32_t& tx_id) const { return false; }

        std::vector<msg::ObdMessage> parse_messages(const StringVector& lines);
    };
}

#endif //CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
