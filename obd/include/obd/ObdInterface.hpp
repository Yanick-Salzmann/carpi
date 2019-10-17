#ifndef CARPI_OBD_OBDINTERFACE_HPP
#define CARPI_OBD_OBDINTERFACE_HPP

#include <memory>
#include <vector>
#include <thread>
#include <chrono>

#include <common_utils/log.hpp>

#include "protocols/AbstractOBDProtocol.hpp"

namespace carpi::bluetooth {
    class BluetoothConnection;
}

namespace carpi::obd {
    class ObdCommand;

    class ObdInterface {
        LOGGER;

        std::shared_ptr<bluetooth::BluetoothConnection> _connection{};
        std::shared_ptr<protocols::AbstractOBDProtocol> _protocol{};

        std::mutex _command_lock{};

        bool _is_in_lower_power_mode = true;

        void send_raw(const std::string& payload);

        template<typename Rep, typename Period>
        std::vector<std::string> send_raw_command(const std::string& command, const std::chrono::duration<Rep, Period>& delay) {
            return send_raw_command(command, std::chrono::duration_cast<std::chrono::seconds>(delay).count());
        }

        std::vector<std::string> send_raw_command(const std::string& command, int32_t delay_seconds = -1);

        void trigger_normal_power();

        std::vector<std::string> read_raw();

        std::vector<msg::ObdMessage> read_messages();

        void initialize();

        bool check_voltage();
        bool try_load_protocol();

        static bool is_ok_message(const std::vector<std::string>& lines, bool allow_multi_line = false);
        static bool contains_in_lines(const std::vector<std::string>& lines, const std::string& search);

    public:
        explicit ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection);

        void send_command(ObdCommand& command);
    };
}

#endif //CARPI_OBD_OBDINTERFACE_HPP
