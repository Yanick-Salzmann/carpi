#ifndef CARPI_OBD_OBDFRAME_HPP
#define CARPI_OBD_OBDFRAME_HPP

#include <string>

namespace carpi::obd::msg {
    class ObdFrame {
        std::string _raw;
        uint32_t _tx_id = 0;

    public:
        explicit ObdFrame(std::string raw) : _raw(std::move(raw)) { }

        [[nodiscard]] uint32_t tx_id() const {
            return _tx_id;
        }
    };
}

#endif //CARPI_OBD_OBDFRAME_HPP
