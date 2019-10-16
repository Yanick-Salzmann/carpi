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
