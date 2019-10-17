#ifndef CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
#define CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP

#include <vector>
#include <string>
#include <map>

#include <obd/msg/ObdMessage.hpp>
#include <obd/msg/ObdFrame.hpp>
#include <obd/EcuType.hpp>
#include <common_utils/log.hpp>

namespace carpi::obd::protocols {
    typedef std::vector<std::string> StringVector;

    class AbstractOBDProtocol {
        LOGGER;

        std::map<uint32_t, EcuType> _ecu_map;

        static void partition_lines(const StringVector& raw_lines, StringVector& obd_lines, StringVector& invalid_lines);

        std::vector<msg::ObdFrame> parse_frames(const StringVector& obd_lines);

        void load_ecu_mapping_from_init_messages(const std::vector<msg::ObdMessage>& init_messages);

    protected:
        AbstractOBDProtocol() = default;

        static bool parse_and_verify_frame_data(const std::string& raw, std::vector<uint8_t>& data, std::size_t min_size, std::size_t max_size);

        void process_init_lines(const StringVector& init_lines);

        virtual bool parse_frame(msg::ObdFrame& frame) = 0;
        virtual bool parse_message(msg::ObdMessage& msg) = 0;

        virtual bool tx_id_engine_constant(uint32_t& tx_id) const { return false; };
        virtual bool tx_id_transmission_constant(uint32_t& tx_id) const { return false; }

    public:
        std::vector<msg::ObdMessage> parse_messages(const StringVector& lines);

        [[nodiscard]] virtual std::string protocol_name() const = 0;
        [[nodiscard]] virtual char protocol_id() const = 0;
    };
}

#endif //CARPI_OBD_ABSTRACTOBDPROTOCOL_HPP
