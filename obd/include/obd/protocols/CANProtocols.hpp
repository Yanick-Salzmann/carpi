#ifndef CARPI_OBD_CANPROTOCOLS_HPP
#define CARPI_OBD_CANPROTOCOLS_HPP

#include "AbstractCANProtocol.hpp"

namespace carpi::obd::protocols {
    class ISO_15765_4_11bit_500k : public AbstractCANProtocol {
    public:
        explicit ISO_15765_4_11bit_500k(const StringVector &initLines) : AbstractCANProtocol(initLines, 11) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 15765-4 (CAN 11/500)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '6';
        }
    };

    class ISO_15765_4_29bit_500k : public AbstractCANProtocol {
    public:
        explicit ISO_15765_4_29bit_500k(const StringVector &initLines) : AbstractCANProtocol(initLines, 29) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 15765-4 (CAN 29/500)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '7';
        }
    };

    class ISO_15765_4_11bit_250k : public AbstractCANProtocol {
    public:
        explicit ISO_15765_4_11bit_250k(const StringVector &initLines) : AbstractCANProtocol(initLines, 11) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 15765-4 (CAN 11/250)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '8';
        }
    };

    class ISO_15765_4_29bit_250k : public AbstractCANProtocol {
    public:
        explicit ISO_15765_4_29bit_250k(const StringVector &initLines) : AbstractCANProtocol(initLines, 29) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "ISO 15765-4 (CAN 29/250)";
        }

        [[nodiscard]] char protocol_id() const override {
            return '9';
        }
    };

    class SAE_J1939 : public AbstractCANProtocol {
    public:
        explicit SAE_J1939(const StringVector &initLines) : AbstractCANProtocol(initLines, 29) {}

        [[nodiscard]] std::string protocol_name() const override {
            return "SAE J1939 (CAN 29/250)";
        }

        [[nodiscard]] char protocol_id() const override {
            return 'A';
        }
    };
}

#endif //CARPI_OBD_CANPROTOCOLS_HPP
