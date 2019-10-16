#ifndef CARPI_OBD_OBDFRAME_HPP
#define CARPI_OBD_OBDFRAME_HPP

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace carpi::obd::msg {
    class ObdFrame {
        std::string _raw;
        std::vector<uint8_t> _data;
        uint32_t _priority = 0;
        uint32_t _tx_id = 0;
        uint32_t _rx_id = 0;
        uint32_t _addr_mode = 0;
        uint32_t _type = 0;
        uint32_t _data_size = 0;
        uint32_t _sequence = 0;

    public:
        explicit ObdFrame(std::string raw) : _raw(std::move(raw)) { }

        [[nodiscard]] uint32_t tx_id() const {
            return _tx_id;
        }

        [[nodiscard]] uint32_t rx_id() const {
            return _rx_id;
        }

        [[nodiscard]] uint32_t priority() const {
            return _priority;
        }

        [[nodiscard]] std::string raw() const {
            return _raw;
        }

        [[nodiscard]] std::vector<uint8_t> data() const {
            return _data;
        }

        [[nodiscard]] uint32_t addr_mode() const {
            return _addr_mode;
        }

        [[nodiscard]] uint32_t type() const {
            return _type;
        }

        [[nodiscard]] uint32_t data_size() const {
            return _data_size;
        }

        [[nodiscard]] uint32_t sequence() const {
            return _sequence;
        }

        ObdFrame& tx_id(uint32_t tx_id) {
            _tx_id = tx_id;
            return *this;
        }

        ObdFrame& rx_id(uint32_t rx_id) {
            _rx_id = rx_id;
            return *this;
        }

        ObdFrame& priority(uint32_t priority) {
            _priority = priority;
            return *this;
        }

        ObdFrame& data(std::vector<uint8_t> data) {
            _data = std::move(data);
            return *this;
        }

        ObdFrame& addr_mode(uint32_t addr_mode) {
            _addr_mode = addr_mode;
            return *this;
        }

        ObdFrame& type(uint32_t type) {
            _type = type;
            return *this;
        }

        ObdFrame& data_size(uint32_t data_size) {
            _data_size = data_size;
            return *this;
        }

        ObdFrame& sequence(uint32_t sequence) {
            _sequence = sequence;
            return *this;
        }

        [[nodiscard]] std::string to_string() const {
            return fmt::format("<ObdFrame raw={}>", _raw);
        }

        template<typename OStream>
        friend OStream& operator << (OStream& os, const ObdFrame& frame) {
            return os << frame.to_string();
        }
    };
}

#endif //CARPI_OBD_OBDFRAME_HPP
