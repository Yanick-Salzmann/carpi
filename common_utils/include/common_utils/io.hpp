#ifndef CARPI_COMMON_UTILS_IO_HPP
#define CARPI_COMMON_UTILS_IO_HPP

#include <cstdint>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace carpi::utils {
    struct little_endian_policy {
    public:
        void operator() (void* buffer, std::size_t size) {

        }
    };

    struct big_endian_policy {
    public:
        void operator() (void* buffer, std::size_t size) {
            auto* ptr = (uint8_t*) buffer;
            std::reverse(ptr, ptr + size);
        }
    };

    class BinaryReader {
        std::vector<uint8_t> _data{};
        std::size_t _position = 0;

    public:
        BinaryReader() = default;

        explicit BinaryReader(std::vector<uint8_t> data) : _data{std::move(data)} {

        }

        [[nodiscard]] const std::vector<uint8_t>& data() const {
            return _data;
        }

        [[nodiscard]] std::size_t available() const {
            return _position >= _data.size() ? 0 : (_data.size() - _position);
        }

        std::size_t seek(std::size_t position) {
            _position = position;
            return _position;
        }

        std::size_t seek_mod(std::ptrdiff_t diff) {
            if (diff < 0 && std::abs(diff) >= _position) {
                _position = 0;
                return 0;
            }

            _position += diff;
            return _position;
        }

        BinaryReader &read(void *buffer, std::size_t size) {
            if (_position + size > _data.size()) {
                throw std::out_of_range{"Attempted to read past the end of a binary stream"};
            }

            memcpy(buffer, &_data[_position], size);
            _position += size;
            return *this;
        }

        template<typename T, typename E = little_endian_policy>
        T read() {
            static E policy;
            T ret{};
            read(&ret, sizeof ret);
            policy(&ret, sizeof ret);
            return ret;
        }

        template<typename T>
        BinaryReader &read(std::vector<T> &data) {
            return read(data.data(), data.size() * sizeof(T));
        }

        template<typename T, typename E = little_endian_policy>
        BinaryReader &read(T &value) {
            static E policy;
            auto& ret = read(&value, sizeof value);
            policy(&value, sizeof value);
            return ret;
        }

        template<typename T>
        BinaryReader &operator>>(T &value) {
            return read(value);
        }

        template<typename T>
        BinaryReader &operator>>(std::vector<T> &data) {
            return read(data);
        }
    };
}

#endif //CARPI_COMMON_UTILS_IO_HPP
