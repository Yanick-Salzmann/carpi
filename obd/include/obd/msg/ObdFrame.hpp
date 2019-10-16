#ifndef CARPI_OBD_OBDFRAME_HPP
#define CARPI_OBD_OBDFRAME_HPP

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace carpi::obd::msg {
    class ObdFrame {
        std::string _raw;
        uint32_t _tx_id = 0;

    public:
        explicit ObdFrame(std::string raw) : _raw(std::move(raw)) { }

        [[nodiscard]] uint32_t tx_id() const {
            return _tx_id;
        }

        [[nodiscard]] std::string raw() const {
            return _raw;
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
