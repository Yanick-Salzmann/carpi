#ifndef CARPI_WIRING_UTILS_BMP280_HPP
#define CARPI_WIRING_UTILS_BMP280_HPP

#include <cstdint>
#include <string>

#include <common_utils/log.hpp>

namespace carpi::wiring {
    struct BMP280Sample {
        float temperature;
        float pressure;
        float altitude;
    };

    class BMP280Sensor {
        LOGGER;

        enum SensorSampling {
            SAMPLING_NONE = 0x00,
            SAMPLING_X1 = 0x01,
            SAMPLING_X2 = 0x02,
            SAMPLING_X4 = 0x03,
            SAMPLING_X8 = 0x04,
            SAMPLING_X16 = 0x05
        };

        enum SensorMode {
            MODE_SLEEP = 0x00,
            MODE_FORCED = 0x01,
            MODE_NORMAL = 0x03,
            MODE_SOFT_RESET_CODE = 0xB6
        };

        enum SensorFilter {
            FILTER_OFF = 0x00,
            FILTER_X2 = 0x01,
            FILTER_X4 = 0x02,
            FILTER_X8 = 0x03,
            FILTER_X16 = 0x04
        };

        enum StandbyDuration {
            STANDBY_MS_1 = 0x00,
            STANDBY_MS_63 = 0x01,
            STANDBY_MS_125 = 0x02,
            STANDBY_MS_250 = 0x03,
            STANDBY_MS_500 = 0x04,
            STANDBY_MS_1000 = 0x05,
            STANDBY_MS_2000 = 0x06,
            STANDBY_MS_4000 = 0x07
        };

        struct Config {
            uint32_t t_sb : 3;
            uint32_t filter : 3;
            uint32_t none : 1;
            uint32_t spi3w_en : 1;

            [[nodiscard]] uint8_t value() const { return (uint32_t) (t_sb << 5u) | (uint32_t) (filter << 2u) | spi3w_en; }
        };

        struct CtrlMeas {
            uint32_t osrs_t : 3;
            uint32_t osrs_p : 3;
            uint32_t mode : 2;

            [[nodiscard]] uint8_t value() const { return (uint32_t) (osrs_t << 5u) | (uint32_t) (osrs_p << 2u) | mode; }
        };

        int32_t _device = 0;

        int32_t _temperature_fine = 0;

        int32_t _coeff_T1 = 0;
        int32_t _coeff_T2 = 0;
        int32_t _coeff_T3 = 0;

        int32_t _coeff_P1 = 0;
        int32_t _coeff_P2 = 0;
        int32_t _coeff_P3 = 0;
        int32_t _coeff_P4 = 0;
        int32_t _coeff_P5 = 0;
        int32_t _coeff_P6 = 0;
        int32_t _coeff_P7 = 0;
        int32_t _coeff_P8 = 0;
        int32_t _coeff_P9 = 0;

        void read_coefficients();

        void set_parameters();

        bool is_calibrating();

        uint8_t read8(uint8_t reg, bool do_throw = false);

        uint16_t read16(uint8_t reg, bool do_throw = false);

        uint16_t read16LE(uint8_t reg, bool do_throw = false);

        int16_t read16SLE(uint8_t reg, bool do_throw = false) { return (int16_t) read16LE(reg, do_throw); }

        uint32_t read24(uint8_t reg, bool do_throw = false);

        void write_data(uint8_t reg, const void *data, std::size_t num_bytes);

        void write_data(uint8_t reg) {
            write_data(reg, nullptr, 0);
        }

        template<std::size_t N>
        void write_data(uint8_t reg, const std::array<uint8_t, N> &data) {
            write_data(reg, data.data(), data.size());
        }

        template<typename T>
        void write_data(uint8_t reg, const T &data) {
            write_data(reg, &data, sizeof data);
        }

        float read_temperature();
        float read_pressure();
        static float read_altitude(float pressure);

    public:
        explicit BMP280Sensor(const std::string &bus = "/dev/i2c-1", uint8_t address = 0x76);

        BMP280Sample sample();
    };
}

#endif //CARPI_WIRING_UTILS_BMP280_HPP
