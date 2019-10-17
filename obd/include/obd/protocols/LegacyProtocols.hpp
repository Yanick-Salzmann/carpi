#ifndef CARPI_OBD_LEGACYPROTOCOLS_HPP
#define CARPI_OBD_LEGACYPROTOCOLS_HPP

#include "AbstractLegacyProtocol.hpp"

namespace carpi::obd::protocols {
    class SAE_J1850_PWM : public AbstractLegacyProtocol {
    public:
        explicit SAE_J1850_PWM(const StringVector &initLines) : AbstractLegacyProtocol(initLines) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "SAE J1850 PWM";
        }

        [[nodiscard]] char protocol_id() const override {
            return '1';
        }
    };

    class SAE_J1850_VPW : public AbstractLegacyProtocol {
    public:
        explicit SAE_J1850_VPW(const StringVector &initLines) : AbstractLegacyProtocol(initLines) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "SAE J1850 VPW";
        }

        [[nodiscard]] char protocol_id() const override {
            return '2';
        }
    };

    class ISO_9141_2 : public AbstractLegacyProtocol {
    public:
        explicit ISO_9141_2(const StringVector &initLines) : AbstractLegacyProtocol(initLines) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 9141-2";
        }

        [[nodiscard]] char protocol_id() const override {
            return '3';
        }
    };

    class ISO_14230_4_5baud : public AbstractLegacyProtocol {
    public:
        explicit ISO_14230_4_5baud(const StringVector &initLines) : AbstractLegacyProtocol(initLines) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 14230-4 (KWP 5BAUD)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '4';
        }
    };

    class ISO_14230_4_fast : public AbstractLegacyProtocol {
    public:
        explicit ISO_14230_4_fast(const StringVector &initLines) : AbstractLegacyProtocol(initLines) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 14230-4 (KWP FAST)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '5';
        }
    };
}

#endif //CARPI_OBD_LEGACYPROTOCOLS_HPP
