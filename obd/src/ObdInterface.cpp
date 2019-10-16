#include "obd/ObdInterface.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"
#include "obd/ObdCommand.hpp"

#include <regex>

namespace carpi::obd {

    ObdInterface::ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection) :
            _connection(std::move(connection)) {

    }

    void ObdInterface::trigger_normal_power() {
        send_raw(" ");
    }

    void ObdInterface::send_raw(const std::string &payload) {
        if(payload.empty()) {
            return;
        }

        std::string actual_payload = payload;
        if(payload[payload.size() - 1] != '\r') {
            actual_payload += '\r';
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

        std::smatch line_match{};
        auto line_string = resp_buffer;
        std::vector<std::string> lines{};

        while(std::regex_search(line_string, line_match, eol_regex)) {
            lines.emplace_back(line_match.str());
            line_string = line_match.suffix();
        }

        return lines;
    }

    void ObdInterface::send_command(ObdCommand &command) {

    }
}