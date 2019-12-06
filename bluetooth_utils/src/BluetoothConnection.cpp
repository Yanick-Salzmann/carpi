
#include "bluetooth_utils/BluetoothConnection.hpp"

#include "bluetooth/bluetooth.h"
#include "bluetooth/rfcomm.h"

#include <common_utils/error.hpp>

namespace carpi::bluetooth {
    LOGGER_IMPL(BluetoothConnection);

    BluetoothConnection::BluetoothConnection(int socket, sockaddr_rc address) : _socket(socket) {
        char addr_name[18]{};
        ba2str(&address.rc_bdaddr, addr_name);
        addr_name[17] = '\0';

        _address.assign(addr_name);
    }

    BluetoothConnection::~BluetoothConnection() {
        close();
    }

    std::ostream &operator<<(std::ostream &os, const BluetoothConnection &conn) {
        return os << conn.to_string();
    }

    bool BluetoothConnection::write_data(const void *data, std::size_t num_bytes) {
        const auto rc = send(_socket, data, num_bytes, 0);
        if (rc < 0) {
            log->warn("Error sending data over connection: {} (errno={})", utils::error_to_string(), errno);
            return false;
        }

        return true;
    }

    void BluetoothConnection::read_data(void *buffer, std::size_t num_bytes) {
        auto cur_ptr = (uint8_t *) buffer;
        auto remaining = num_bytes;

        while (remaining > 0) {
            const auto num_read = recv(_socket, cur_ptr, remaining, 0);
            if (num_read <= 0) {
                log->error("Error reading data from connection: {} (errno={})", utils::error_to_string(), errno);
                throw std::runtime_error("Error reading from connection");
            }

            cur_ptr += num_read;
            remaining -= num_read;
        }
    }

    void BluetoothConnection::close() {
        if (!_is_open) {
            return;
        }

        shutdown(_socket, SHUT_RDWR);
        ::close(_socket);

        _is_open = false;
    }

    std::size_t BluetoothConnection::read_some(void *buffer, std::size_t max_bytes) {
        const auto num_read = recv(_socket, buffer, max_bytes, 0);
        if(num_read < 0 && errno != EAGAIN && errno != ETIMEDOUT) {
            log->error("Error reading any data: {} (errno={})", utils::error_to_string(), errno);
            throw std::runtime_error("Error reading any data");
        } else if(errno == EAGAIN) {
            return 0;
        } else if(num_read <= 0) {
            return 0;
        }

        return static_cast<std::size_t>(num_read);
    }
}