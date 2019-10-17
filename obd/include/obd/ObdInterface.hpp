#ifndef CARPI_OBD_OBDINTERFACE_HPP
#define CARPI_OBD_OBDINTERFACE_HPP

#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <map>

#include <common_utils/log.hpp>

#include "protocols/AbstractOBDProtocol.hpp"
#include "protocols/LegacyProtocols.hpp"
#include "protocols/CANProtocols.hpp"

namespace carpi::bluetooth {
    class BluetoothConnection;
}

namespace carpi::obd {
    class ObdCommand;

    class ObdInterface {
        LOGGER;

        typedef std::map<char, std::function<std::shared_ptr<protocols::AbstractOBDProtocol>(const protocols::StringVector &)>> ProtocolMap;

#define PROTO_ENTRY(chr, type) { chr, [](const protocols::StringVector& init_lines) { return std::make_shared<protocols::type>(init_lines); } }

        ProtocolMap _protocol_map = {
                // Legacy protocols
                PROTO_ENTRY('1', SAE_J1850_PWM),
                PROTO_ENTRY('2', SAE_J1850_VPW),
                PROTO_ENTRY('3', ISO_9141_2),
                PROTO_ENTRY('4', ISO_14230_4_5baud),
                PROTO_ENTRY('5', ISO_14230_4_fast),

                // CAN protocols
                PROTO_ENTRY('6', ISO_15765_4_11bit_500k),
                PROTO_ENTRY('7', ISO_15765_4_29bit_500k),
                PROTO_ENTRY('8', ISO_15765_4_11bit_250k),
                PROTO_ENTRY('9', ISO_15765_4_29bit_250k),
                PROTO_ENTRY('A', SAE_J1939)
        };

#undef PROTO_ENTRY

        std::vector<char> _guessed_protocols = {'6', '8', '1', '7', '9', '2', '3', '4', '5', 'A'};

        std::shared_ptr<bluetooth::BluetoothConnection> _connection{};
        std::shared_ptr<protocols::AbstractOBDProtocol> _protocol{};

        std::mutex _command_lock{};

        bool _is_in_lower_power_mode = true;

        void send_raw(const std::string &payload);

        template<typename Rep, typename Period>
        std::vector<std::string> send_raw_command(const std::string &command, const std::chrono::duration<Rep, Period> &delay) {
            return send_raw_command(command, std::chrono::duration_cast<std::chrono::seconds>(delay).count());
        }

        std::vector<std::string> send_raw_command(const std::string &command, int32_t delay_seconds = -1);

        void trigger_normal_power();

        std::vector<std::string> read_raw();

        std::vector<msg::ObdMessage> read_messages();

        void initialize();

        bool check_voltage();

        bool try_load_protocol();

        static bool is_ok_message(const std::vector<std::string> &lines, bool allow_multi_line = false);

        static bool contains_in_lines(const std::vector<std::string> &lines, const std::string &search);

        static std::string combine_lines_for_output(const std::vector<std::string> &lines);

    public:
        explicit ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection);

        void send_command(ObdCommand &command);
    };
}

#endif //CARPI_OBD_OBDINTERFACE_HPP
