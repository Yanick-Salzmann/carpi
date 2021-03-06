#include "obd/ObdInterface.hpp"
#include "bluetooth_utils/bluetooth_connection.hpp"
#include "obd/ObdCommand.hpp"
#include "obd/msg/ObdMessage.hpp"
#include "obd/ObdCommandList.hpp"

#include <regex>
#include <iomanip>
#include <common_utils/any.hpp>

namespace carpi::obd {
    LOGGER_IMPL(ObdInterface);

    ObdInterface::ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection) :
            _connection(std::move(connection)) {
        initialize();
    }

    void ObdInterface::trigger_normal_power(bool await_response) {
        log->info("Triggering high power mode");
        _connection->write_data(" ", 1);
        if (await_response) {
            read_raw();
        }
        _is_in_lower_power_mode = false;
    }

    void ObdInterface::send_raw(const std::string &payload) {
        if (payload.empty()) {
            return;
        }

        std::string actual_payload = payload;
        if (payload[payload.size() - 1] != '\r') {
            actual_payload += '\r';
        }

        if (_is_in_lower_power_mode) {
            trigger_normal_power(false);
        }

        log->debug("TO ELM327: {} (={} bytes)", actual_payload, actual_payload.size());
        _connection->write_data(actual_payload.c_str(), actual_payload.size());
    }

    std::vector<std::string> ObdInterface::read_raw(int32_t timeout) {
        uint8_t read_chunk[0x1000]{};
        std::vector<uint8_t> resp_data{};
        std::string resp_buffer{};

        do {
            const auto num_read = _connection->read_some(read_chunk, sizeof read_chunk);
            if (num_read == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            resp_data.insert(resp_data.end(), read_chunk, read_chunk + num_read);
            resp_buffer.assign(resp_data.begin(), resp_data.end());

            if (resp_buffer.find('>') != std::string::npos) {
                break;
            }
        } while (true);

        static std::regex eol_regex{"[\r\n]"};

        std::vector<std::string> lines{};

        std::sregex_token_iterator itr{resp_buffer.begin(), resp_buffer.end(), eol_regex, -1};
        std::sregex_token_iterator end{};

        for (; itr != end; ++itr) {
            const auto content = (*itr).str();

            if (content.empty() || (content.size() == 1 && content[0] == '>')) {
                continue;
            }

            lines.emplace_back(*itr);
        }

        log->debug("FROM ELM: {}", combine_lines_for_output(lines));

        return lines;
    }

    std::vector<msg::ObdMessage> ObdInterface::read_messages() {
        return _protocol->parse_messages(read_raw());
    }

    bool ObdInterface::send_command(const ObdCommand &command, utils::Any& response) {
        std::lock_guard<std::mutex> l{_command_lock};

        std::stringstream command_string;
        command_string << std::setw(2) << std::setfill('0') << command.service_id() << command.pid();
        send_raw(command_string.str());
        const auto raw_messages = read_messages();
        const auto message = raw_messages[0];
        const auto data = message.data();
        if(command.decoder()->response_size() + 2 > data.size()) {
            log->warn("Response for OBD command is supposed to have at least {} bytes, but only {} were returned", command.decoder()->response_size(), data.size());
            return false;
        }

        std::vector<uint8_t> actual_data{data.begin() + 2, data.end()};
        response = command.decoder()->decode(actual_data);
        return true;
    }

    void ObdInterface::initialize() {
        initialize_commands();

        log->info("Trying to initialize OBD protocol");

        trigger_normal_power(false);
        std::this_thread::sleep_for(std::chrono::seconds{2});

        send_raw_command("ATZ", 1);

        if (!is_ok_message(send_raw_command("ATE0"), true)) {
            log->error("Error invoking ATE0 command. Expected 'OK' response");
            throw std::runtime_error("Error initializing ELM327");
        }

        const auto header_resp = send_raw_command("ATH1");

        if (header_resp.empty()) {
            log->error("Error invoking ATH1 command. Expected some response");
            throw std::runtime_error("Error initializing ELM327");
        }

        if (!is_ok_message(send_raw_command("ATL0"), true)) {
            log->error("Error invoking ATL0 command. Expected 'OK' response");
            throw std::runtime_error("Error initializing ELM327");
        }

        if (!check_voltage()) {
            log->error("Voltage check failed");
            throw std::runtime_error("Voltage check failed");
        }

        if (!try_load_protocol()) {
            log->error("Error finding any compatible ELM327 underlying protocol");
            throw std::runtime_error("Error negotiating protocol with car");
        }

        log->info("Connected to ELM327 chip and car using protocol '{}'", _protocol->protocol_name());
    }

    std::vector<std::string> ObdInterface::send_raw_command(const std::string &command, int32_t delay_seconds) {
        send_raw(command);
        if (delay_seconds > 0) {
            std::this_thread::sleep_for(std::chrono::seconds{delay_seconds});
        }

        return read_raw();
    }

    bool ObdInterface::is_ok_message(const std::vector<std::string> &lines, bool allow_multi_line) {
        if (lines.empty()) {
            return false;
        }

        if (!allow_multi_line) {
            return lines.size() == 1 && lines[0] == "OK";
        } else {
            return contains_in_lines(lines, "OK");
        }
    }

    bool ObdInterface::check_voltage() {
        const auto response = send_raw_command("AT RV");
        if (response.size() != 1 || response[0].empty()) {
            return false;
        }

        try {
            auto voltage_string = response[0];
            if(voltage_string[0] == 'v' || voltage_string[0] == 'V') {
                voltage_string = voltage_string.substr(1); // skip v prefix
            }

            log->info("Trying to parse OBD voltage from: {}", voltage_string);
            const auto voltage = utils::lexical_cast<float>(voltage_string);
            return voltage >= 6.0f;
        } catch (std::bad_cast &) {
            log->warn("Incorrect voltage response received: {}", response[0]);
            return false;
        }
    }

    bool ObdInterface::try_load_protocol() {
        send_raw_command("ATSP0");
        auto init_lines = send_raw_command("0100");

        if (contains_in_lines(init_lines, "UNABLE TO CONNECT")) {
            log->error("Unable to launch SEARCH PROTOCOL (0100) request to ELM327. Response was {}",
                       combine_lines_for_output(init_lines));
            return false;
        }

        auto response = send_raw_command("ATDPN");
        if (response.size() != 1) {
            log->error("Expected a single line response from 'ATDPN' ELM327 request, but got: {}",
                       combine_lines_for_output(response));
            return false;
        }

        auto protocol_line = response[0];
        if (protocol_line.size() > 1 && protocol_line[0] == 'A') {
            protocol_line = protocol_line.substr(1);
        }

        auto protocol_id = protocol_line[0];
        const auto proto_itr = _protocol_map.find(protocol_id);
        if (proto_itr != _protocol_map.end()) {
            _protocol = proto_itr->second(init_lines);
            return true;
        } else {
            for (const auto &guessed_proto : _guessed_protocols) {
                send_raw_command(std::string{"ATTP"} + guessed_proto);
                init_lines = send_raw_command("0100");
                if (!contains_in_lines(init_lines, "UNABLE TO CONNECT")) {
                    _protocol = _protocol_map[guessed_proto](init_lines);
                    return true;
                }
            }
        }

        log->error("Unable to find any detectable ELM327 underlying protocol, out of luck...");
        return false;
    }

    std::string ObdInterface::combine_lines_for_output(const std::vector<std::string> &lines) {
        std::stringstream stream;
        stream << "[";
        auto is_first = true;
        for (const auto &line : lines) {
            if (is_first) {
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
        for (const auto &line : lines) {
            if (line.find(search) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    std::vector<msg::ObdMessage> ObdInterface::dispatch_raw_command(const std::string &command) {
        send_raw_command(command);
        return read_messages();
    }
}