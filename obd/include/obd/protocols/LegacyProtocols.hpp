#ifndef CARPI_OBD_LEGACYPROTOCOLS_HPP
#define CARPI_OBD_LEGACYPROTOCOLS_HPP

#include "AbstractLegacyProtocol.hpp"

namespace carpi::obd::protocols {
    class SAE_J1850_PWM : public AbstractLegacyProtocol {
    public:
        [[nodiscard]] std::string protocol_name() const override {
            return "SAE J1850 PWM";
        }

        [[nodiscard]] std::string protocol_id() const override {
            return "1";
        }
    };

    class SAE_J1850_VPW : public AbstractLegacyProtocol {
    public:
        [[nodiscard]] std::string protocol_name() const override {
            return "SAE J1850 VPW";
        }

        [[nodiscard]] std::string protocol_id() const override {
            return "2";
        }
    };

    class ISO_9141_2 : public AbstractLegacyProtocol {
    public:
        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 9141-2";
        }

        [[nodiscard]] std::string protocol_id() const override {
            return "3";
        }
    };

    class ISO_14230_4_5baud : public AbstractLegacyProtocol {
    public:
        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 14230-4 (KWP 5BAUD)";
        }

        [[nodiscard]] std::string protocol_id() const override {
            return "4";
        }
    };

    class ISO_14230_4_fast : public AbstractLegacyProtocol {
    public:
        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 14230-4 (KWP FAST)";
        }

        [[nodiscard]] std::string protocol_id() const override {
            return "5";
        }
    };
}

#endif //CARPI_OBD_LEGACYPROTOCOLS_HPP
