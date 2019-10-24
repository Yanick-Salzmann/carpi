#ifndef CARPI_IPC_COMMON_IPCPACKAGE_HPP
#define CARPI_IPC_COMMON_IPCPACKAGE_HPP

#include "Opcodes.hpp"
#include <vector>
#include <cstdint>
#include <common_utils/log.hpp>

namespace carpi::ipc {
    class IpcPackage {
        LOGGER;

        Opcodes _opcode;
        std::vector<uint8_t> _data;
        std::size_t _read_pos = 0;

    public:
        IpcPackage(Opcodes opcode, std::vector<uint8_t> data) : _opcode{opcode}, _data{std::move(data)} {

        }

        static IpcPackage read_from_socket(int socket);

        void read(void* buffer, std::size_t size);

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
    };
}

#endif //CARPI_IPC_COMMON_IPCPACKAGE_HPP
