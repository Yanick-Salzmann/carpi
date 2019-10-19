#ifndef CARPI_OBD_BITARRAYRESPONSECODEC_HPP
#define CARPI_OBD_BITARRAYRESPONSECODEC_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>

#include "ResponseConverter.hpp"

namespace carpi::obd::response {
    class BitArray {
        std::vector<uint8_t> _data;

    public:
        explicit BitArray(std::vector<uint8_t> data) : _data(std::move(data)) {

        }

        bool operator [] (std::size_t index) const {
            const auto bit_index = index % 8u;
            const auto byte_index = index / 8u;

            if(byte_index >= _data.size()) {
                throw std::out_of_range("Attempted to read out of range of bit array");
            }

            return ((_data[byte_index] >> bit_index) & 1u) != 0;
        }
    };

    class BitArrayResponseCodec : public IResponseConverter {
        uint32_t _bit_count;
        uint32_t _byte_count;

    public:
        explicit BitArrayResponseCodec(uint32_t num_bits) : _bit_count(num_bits), _byte_count((num_bits + 7u) / 8u) { }

        [[nodiscard]] size_t response_size() const override {
            return _byte_count;
        }

        utils::Any decode(const std::vector<uint8_t> &data) override {
            if(data.size() < _byte_count) {
                throw std::invalid_argument("Response too small");
            }

            std::vector<uint8_t> actual_data{};
            if(data.size() != _byte_count) {
                actual_data.assign(data.begin(), data.begin() + _byte_count);
            } else {
                actual_data = data;
            }

            return utils::Any(BitArray(actual_data));
        }
    };
}

#endif //CARPI_OBD_BITARRAYRESPONSECODEC_HPP
