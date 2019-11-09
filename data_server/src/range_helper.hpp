#ifndef CARPI_DATA_SERVER_RANGE_HELPER_HPP
#define CARPI_DATA_SERVER_RANGE_HELPER_HPP

#include <string>
#include <vector>
#include <common_utils/log.hpp>

namespace carpi::data {
    enum class RangeType {
        OPEN_END,
        BOUNDED,
        SUFFIX
    };

    struct RequestRange {
        RangeType type;
        std::size_t start;
        std::size_t end;

        [[nodiscard]] std::string to_string() const;
    };

    class Range {
        std::string _unit{};

        std::vector<RequestRange> _ranges;

        static RequestRange process_range(const std::string& range);

    public:
        explicit Range(const std::string& range_header);

        void print(utils::Logger& logger);

        [[nodiscard]] const std::vector<RequestRange>& ranges() const {
            return _ranges;
        }
    };
}

#endif //CARPI_DATA_SERVER_RANGE_HELPER_HPP
