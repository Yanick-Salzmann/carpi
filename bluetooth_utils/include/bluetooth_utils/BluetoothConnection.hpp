#ifndef CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP
#define CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP

#include <sys/socket.h>
#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <sstream>

#include "common_utils/log.hpp"

namespace carpi::bluetooth {
    class BluetoothConnection {
        LOGGER;

        friend std::ostream &operator<<(std::ostream &os, const BluetoothConnection &conn);

        int _socket;
        std::string _address{};

        bool _is_open = true;

    public:
        explicit BluetoothConnection(int socket);

        BluetoothConnection(const BluetoothConnection &) = delete;

        BluetoothConnection(BluetoothConnection &&) = delete;

        ~BluetoothConnection();

        [[nodiscard]] std::string to_string() const {
            std::stringstream stream;
            stream << "<BluetoothConnection address=" << _address << ">";
            return stream.str();
        }

        void operator=(const BluetoothConnection &) = delete;

        void operator=(BluetoothConnection &&) = delete;

        void close();

        void write_data(const void *data, std::size_t num_bytes);

        void read_data(void *buffer, std::size_t num_bytes);

        template<typename T>
        BluetoothConnection &operator<<(const T &value) {
            write_data(&value, sizeof value);
            return *this;
        }

        BluetoothConnection &operator<<(const char *value) {
            write_data(value, strlen(value));
            *this << char{0};
            return *this;
        }

        template<typename T, std::size_t N>
        BluetoothConnection &operator<<(const std::array<T, N> &data) {
            write_data(data.data(), N * sizeof(T));
            return *this;
        }

        template<typename T>
        BluetoothConnection &operator<<(const std::vector<T> &data) {
            write_data(data.data(), data.size() * sizeof(T));
            return *this;
        }

        template<typename T>
        BluetoothConnection &operator>>(T &value) {
            read_data(&value, sizeof(T));
            return *this;
        }

        template<typename T, std::size_t N>
        BluetoothConnection &operator>>(std::array<T, N> &data) {
            read_data(data.data(), N * sizeof(T));
            return *this;
        }

        template<typename T>
        BluetoothConnection &operator>>(std::vector<T> &data) {
            read_data(data.data(), data.size() * sizeof(T));
            return *this;
        }

        [[nodiscard]] int fd() const {
            return _socket;
        }
    };

    template<>
    inline BluetoothConnection& BluetoothConnection::operator<< <std::string>(const std::string& value) {
        write_data(value.c_str(), value.size() + 1);
        return *this;
    }

    template<>
    inline BluetoothConnection& BluetoothConnection::operator>> <std::string>(std::string &value) {
        char char_elem{};
        std::vector<char> buffer{};
        do {
            (*this) >> char_elem;
            buffer.emplace_back(char_elem);
        } while (char_elem != 0);

        value.assign(buffer.data());
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const BluetoothConnection &conn);
}

#endif //CARPI_BLUETOOTH_UTILS_BLUETOOTHCONNECTION_HPP
