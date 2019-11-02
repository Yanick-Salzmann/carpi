#include "wiring_utils/bmp280.hpp"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include <common_utils/error.hpp>

namespace carpi::wiring {
    LOGGER_IMPL(BMP280Sensor);

    enum {
        BMP280_REGISTER_DIG_T1 = 0x88,
        BMP280_REGISTER_DIG_T2 = 0x8A,
        BMP280_REGISTER_DIG_T3 = 0x8C,
        BMP280_REGISTER_DIG_P1 = 0x8E,
        BMP280_REGISTER_DIG_P2 = 0x90,
        BMP280_REGISTER_DIG_P3 = 0x92,
        BMP280_REGISTER_DIG_P4 = 0x94,
        BMP280_REGISTER_DIG_P5 = 0x96,
        BMP280_REGISTER_DIG_P6 = 0x98,
        BMP280_REGISTER_DIG_P7 = 0x9A,
        BMP280_REGISTER_DIG_P8 = 0x9C,
        BMP280_REGISTER_DIG_P9 = 0x9E,
        BMP280_REGISTER_CHIPID = 0xD0,
        BMP280_REGISTER_VERSION = 0xD1,
        BMP280_REGISTER_SOFTRESET = 0xE0,
        BMP280_REGISTER_CAL26 = 0xE1,
        BMP280_REGISTER_CONTROL = 0xF4,
        BMP280_REGISTER_CONFIG = 0xF5,
        BMP280_REGISTER_PRESSUREDATA = 0xF7,
        BMP280_REGISTER_TEMPDATA = 0xFA,
    };

    BMP280Sensor::BMP280Sensor(const std::string &bus, uint8_t address) {
        _device = open(bus.c_str(), O_RDWR);
        if (_device < 0) {
            log->warn("Error opening i2c bus '{}': {} (errno={})", bus, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening i2c bus"};
        }

        if (ioctl(_device, I2C_SLAVE, address) != 0) {
            log->warn("Error sending ioctl I2C_SLAVE request to i2c bus: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting I2C_SLAVE on i2c bus"};
        }

        read_coefficients();
        set_parameters();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    BMP280Sample BMP280Sensor::sample() {
        const auto temp = read_temperature();
        const auto pressure = read_pressure();
        return {
            .temperature = temp,
            .pressure = pressure / 100.0f,
            .altitude = read_altitude(pressure)
        };
    }

    void BMP280Sensor::read_coefficients() {
        // temperature coeffs
        _coeff_T1 = read16LE(BMP280_REGISTER_DIG_T1, true);
        _coeff_T2 = read16SLE(BMP280_REGISTER_DIG_T2, true);
        _coeff_T3 = read16SLE(BMP280_REGISTER_DIG_T3, true);

        // pressure coeffs
        _coeff_P1 = read16LE(BMP280_REGISTER_DIG_P1, true);
        _coeff_P2 = read16SLE(BMP280_REGISTER_DIG_P2, true);
        _coeff_P3 = read16SLE(BMP280_REGISTER_DIG_P3, true);
        _coeff_P4 = read16SLE(BMP280_REGISTER_DIG_P4, true);
        _coeff_P5 = read16SLE(BMP280_REGISTER_DIG_P5, true);
        _coeff_P6 = read16SLE(BMP280_REGISTER_DIG_P6, true);
        _coeff_P7 = read16SLE(BMP280_REGISTER_DIG_P7, true);
        _coeff_P8 = read16SLE(BMP280_REGISTER_DIG_P8, true);
        _coeff_P9 = read16SLE(BMP280_REGISTER_DIG_P9, true);
    }

    void BMP280Sensor::set_parameters() {
        Config config_reg{};
        config_reg.filter = FILTER_OFF;
        config_reg.t_sb = STANDBY_MS_1;

        write_data(BMP280_REGISTER_CONFIG, config_reg.value());

        CtrlMeas meas_reg{};
        meas_reg.mode = MODE_NORMAL;
        meas_reg.osrs_p = SAMPLING_X16;
        meas_reg.osrs_t = SAMPLING_X16;

        write_data(BMP280_REGISTER_CONTROL, meas_reg.value());
    }

    uint16_t BMP280Sensor::read16(uint8_t reg, bool do_throw) {
        write_data(reg);

        uint16_t ret_val = 0;
        if (read(_device, &ret_val, sizeof ret_val) != sizeof ret_val) {
            log->warn("Error reading uint16 value from BMP sensor: {} (errno={})", utils::error_to_string(errno), errno);
            if (do_throw) {
                throw std::runtime_error{"Error reading uint16 value from BMP sensor"};
            }
            return 0;
        }

        return ret_val;
    }

    uint16_t BMP280Sensor::read16LE(uint8_t reg, bool do_throw) {
        const auto be_value = read16(reg, do_throw);
        return (uint16_t) (be_value >> 8u) | (uint16_t) (be_value << 8u);
    }

    uint32_t BMP280Sensor::read24(uint8_t reg, bool do_throw) {
        write_data(reg);
        uint8_t byte_vals[3]{};
        if (::read(_device, byte_vals, sizeof byte_vals) != sizeof byte_vals) {
            log->warn("Error reading 24 bit value from BMP sensor: {} (errno={})", utils::error_to_string(errno), errno);

            if (do_throw) {
                throw std::runtime_error{"Error reading 24 bit value from BMP sensor"};
            }

            return 0;
        }

        return ((uint32_t) byte_vals[2]) | (((uint32_t) byte_vals[1]) << 8u) | (((uint32_t) byte_vals[0]) << 16u);
    }

    void BMP280Sensor::write_data(uint8_t reg, const void *data, std::size_t num_bytes) {
        ::write(_device, &reg, sizeof reg);
        if (num_bytes > 0) {
            ::write(_device, data, num_bytes);
        }
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

    float BMP280Sensor::read_temperature() {
        int32_t adc_t = read24(BMP280_REGISTER_TEMPDATA);
        adc_t >>= 4;

        int32_t val1 = ((((adc_t >> 3) - ((int32_t) _coeff_T1 << 1))) *
                        ((int32_t) _coeff_T2)) >> 11;

        int32_t val2 = (((((adc_t >> 4) - ((int32_t) _coeff_T1)) *
                          ((adc_t >> 4) - ((int32_t) _coeff_T1))) >> 12) *
                        ((int32_t) _coeff_T3)) >> 14;

        _temperature_fine = val1 + val2;
        const auto T = (_temperature_fine * 5 + 128) >> 8;
        return T / 100.0f;
    }

    float BMP280Sensor::read_pressure() {
        int32_t adc_p = read24(BMP280_REGISTER_PRESSUREDATA);
        adc_p >>= 4;

        int64_t var1 = ((int64_t) _temperature_fine) - 128000;
        int64_t var2 = var1 * var1 * (int64_t) _coeff_P6;
        var2 = var2 + ((var1 * (int64_t) _coeff_P5) << 17);
        var2 = var2 + (((int64_t) _coeff_P4) << 35);
        var1 = ((var1 * var1 * (int64_t) _coeff_P3) >> 8) + ((var1 * (int64_t) _coeff_P2) << 12);
        var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) _coeff_P1) >> 33;

        if (var1 == 0) {
            return 0.0f;
        }

        int64_t p = 1048576 - adc_p;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((int64_t) _coeff_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t) _coeff_P8) * p) >> 19;

        p = ((p + var1 + var2) >> 8) + (((int64_t) _coeff_P7) << 4);

        return p / 256.0f;
    }

    float BMP280Sensor::read_altitude(float pressure) {
        pressure /= 100.0f;
        return 44330.0f * (1.0f - powf(pressure / 1013.25f, 0.1903f));
    }

#pragma clang diagnostic pop
}