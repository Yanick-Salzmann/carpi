#include "obd/ObdInterface.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"
#include "obd/ObdCommand.hpp"
#include "obd/msg/ObdMessage.hpp"

#include <regex>
#include <iomanip>
#include <common_utils/any.hpp>

namespace carpi::obd {
    LOGGER_IMPL(ObdInterface);

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

        log->debug("TO ELM327: {}", actual_payload);
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

        log->debug("FROM ELM327: {}", resp_buffer);

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

        if(!is_ok_message(send_raw_command("ATE0"), true)) {
            log->error("Error invoking ATE0 command. Expected 'OK' response");
            throw std::runtime_error("Error initializing ELM327");
        }

        if(!is_ok_message(send_raw_command("ATH1"))) {
            log->error("Error invoking ATH1 command. Expected 'OK' response");
            throw std::runtime_error("Error initializing ELM327");
        }

        if(!is_ok_message(send_raw_command("ATL0"))) {
            log->error("Error invoking ATL0 command. Expected 'OK' response");
            throw std::runtime_error("Error initializing ELM327");
        }

        if(!check_voltage()) {
            log->error("Voltage check failed");
            throw std::runtime_error("Voltage check failed");
        }
    }

    std::vector<std::string> ObdInterface::send_raw_command(const std::string &command, int32_t delay_seconds) {
        send_raw(command);
        if(delay_seconds > 0) {
            std::this_thread::sleep_for(std::chrono::seconds{delay_seconds});
        }

        return read_raw();
    }

    bool ObdInterface::is_ok_message(const std::vector<std::string> &lines, bool allow_multi_line) {
        if(lines.empty()) {
            return false;
        }

        if(!allow_multi_line) {
            return lines.size() == 1 && lines[0] == "OK";
        } else {
            return contains_in_lines(lines, "OK");
        }
    }

    bool ObdInterface::check_voltage() {
        const auto response = send_raw_command("AT RV");
        if(response.size() != 1 || response[0].empty()) {
            return false;
        }

        try {
            const auto voltage = utils::lexical_cast<float>(response[0].substr(1)); // skip the 'v' prefix
            return voltage >= 6.0f;
        } catch (std::bad_cast&) {
            log->warn("Incorrect voltage response received: {}", response[0]);
            return false;
        }
    }

    bool ObdInterface::try_load_protocol() {
        send_raw_command("ATSP0");
        auto init_lines = send_raw_command("0100");

        if(contains_in_lines(init_lines, "UNABLE TO CONNECT")) {
            log->error("Unable to launch SEARCH PROTOCOL (0100) request to ELM327. Response was {}", combine_lines_for_output(init_lines));
            return false;
        }

        auto response = send_raw_command("ATDPN");
        if(response.size() != 1) {
            log->error("Expected a single line response from 'ATDPN' ELM327 request, but got: {}", combine_lines_for_output(response));
            return false;
        }

        return false;
    }

    std::string ObdInterface::combine_lines_for_output(const std::vector<std::string> &lines) {
        std::stringstream stream;
        stream << "[";
        auto is_first = true;
        for(const auto& line : lines) {
            if(is_first) {
                is_first = false;
            } else {
                stream << ", ";
            }

            stream << "'" << line << "'";
        }

        stream << "]";

        return stream.str();
    }

    bool ObdInterface::contains_in_lines(const std::vector<std::string> &lines, const std::string &search) {
        for(const auto& line : lines) {
            if(line.find(search) != std::string::npos) {
                return true;
            }
        }

        return false;
    }
}