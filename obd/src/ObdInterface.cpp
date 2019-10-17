#include "obd/ObdInterface.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"
#include "obd/ObdCommand.hpp"
#include "obd/msg/ObdMessage.hpp"

#include <regex>
#include <iomanip>

namespace carpi::obd {

    ObdInterface::ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection) :
            _connection(std::move(connection)) {
        initialize();
    }

    void ObdInterface::trigger_normal_power() {
        send_raw(" ");
        read_raw();
        _is_in_lower_power_mode = false;
    }

    void ObdInterface::send_raw(const std::string &payload) {
        if(payload.empty()) {
            return;
        }

        std::string actual_payload = payload;
        if(payload[payload.size() - 1] != '\r') {
            actual_payload += '\r';
        }

        if(_is_in_lower_power_mode) {
            trigger_normal_power();
        }

        _connection->write_data(actual_payload.c_str(), actual_payload.size());
    }

    std::vector<std::string> ObdInterface::read_raw() {
        std::string resp_buffer{};
        uint8_t chr{};
        uint8_t last_chr = '\0';
        do {
            (*_connection) >> chr;
            resp_buffer += chr;
            if(chr == '>') {
                break;
            }

            if(chr == 'K' && last_chr == 'O') {
                _is_in_lower_power_mode = true;
                break;
            }

            last_chr = chr;
        } while(true);

        static std::regex eol_regex{"[\r\n]"};

        std::vector<std::string> lines{};

        std::sregex_token_iterator itr{resp_buffer.begin(), resp_buffer.end(), eol_regex, -1};
        std::sregex_token_iterator end{};

        for(; itr != end; ++itr) {
            lines.emplace_back(*itr);
        }

        return lines;
    }

    std::vector<msg::ObdMessage> ObdInterface::read_messages() {
        return _protocol->parse_messages(read_raw());
    }

    void ObdInterface::send_command(ObdCommand &command) {
        std::stringstream command_string;
        command_string << std::setw(2) << std::setfill('0') << command.service_id() << command.pid();
        send_raw(command_string.str());
        const auto raw_messages = read_messages();

    }

    void ObdInterface::initialize() {
        trigger_normal_power();
        send_raw_command("ATZ", 1);
    }

    std::vector<std::string> ObdInterface::send_raw_command(const std::string &command, int32_t delay_seconds) {
        send_raw(command);
        if(delay_seconds > 0) {
            std::this_thread::sleep_for(std::chrono::seconds{delay_seconds});
        }

        return read_raw();
    }
}