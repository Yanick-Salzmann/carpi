#ifndef CARPI_IPC_COMMON_IPCPACKAGE_HPP
#define CARPI_IPC_COMMON_IPCPACKAGE_HPP

#include "opcodes.hpp"
#include <vector>
#include <cstdint>
#include <common_utils/log.hpp>

namespace carpi::ipc {
    class IpcPackage {
        LOGGER;

        Opcodes _opcode;
        std::vector<uint8_t> _data;
        std::size_t _read_pos = 0;
        std::size_t _write_pos = 0;

    public:
        IpcPackage(Opcodes opcode, std::vector<uint8_t> data) : _opcode{opcode}, _data{std::move(data)} {

        }

        explicit IpcPackage(Opcodes opcode) : _opcode(opcode) { }

        static IpcPackage read_from_socket(int socket);
        static IpcPackage read_from_mq(int mq);

        void read(void* buffer, std::size_t size);
        void write(const void* buffer, std::size_t size);

        void send_to_mq(int mq) const;

        template<typename T>
        T read() {
            T ret;
            read(&ret, sizeof(T));
            return ret;
        }

        template<typename T>
        IpcPackage& read(T& value) {
            read(&value, sizeof value);
            return *this;
        }

        template<typename T>
        IpcPackage& operator >> (T& value) {
            return read(value);
        }

        template<typename T> IpcPackage& write(const T& value) {
            write(&value, sizeof value);
            return *this;
        }

        template<typename T>
        IpcPackage& operator << (const T& value) {
            return write(value);
        }

        [[nodiscard]] Opcodes opcode() const {
            return _opcode;
        }
    };
}

#endif //CARPI_IPC_COMMON_IPCPACKAGE_HPP
