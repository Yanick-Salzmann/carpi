#include "range_helper.hpp"
#include <stdexcept>
#include <common_utils/string.hpp>
#include <sstream>

namespace carpi::data {

    Range::Range(const std::string &range_header) {
        const auto unit_end = range_header.find('=');
        if(unit_end == std::string::npos) {
            throw std::runtime_error{"Invalid range header. No unit defined (no '=' found)."};
        }

        _unit = utils::trim(range_header.substr(0, unit_end));

        const auto remainder = range_header.substr(unit_end + 1);

        auto last_pos = 0;
        auto next_end = remainder.find(',');
        while(next_end != std::string::npos) {
            const auto range = utils::trim(remainder.substr(last_pos, next_end));
            _ranges.emplace_back(process_range(range));

            last_pos = next_end + 1;
            next_end = remainder.find(',', last_pos);
        }

        if(last_pos < remainder.size()) {
            const auto range = utils::trim(remainder.substr(last_pos));
            _ranges.emplace_back(process_range(range));
        }
    }

    RequestRange Range::process_range(const std::string &range) {
        if(range.empty()) {
            throw std::runtime_error{"Invalid empty range specified"};
        }

        if(range[0] == '-') {
            return {
                .type = RangeType::SUFFIX,
                .start = std::stoull(range.substr(1))
            };
        } else {
            const auto dash = range.find('-');
            const auto start = range.substr(0, dash);
            const auto end = range.substr(dash + 1);
            return {
                .type = end.empty() ? RangeType::OPEN_END : RangeType::BOUNDED,
                .start = (std::size_t) std::stoull(start),
                .end = end.empty() ? 0 : std::stoull(end)
            };
        }
    }

    void Range::print(utils::Logger &logger) {
        std::stringstream stream;
        stream << "<Ranges unit=" << _unit;
        for(const auto& range : _ranges) {
            switch(range.type) {
                case RangeType::SUFFIX: {
                    stream << " type=SUFFIX start=" << range.start;
                    break;
                }

                case RangeType::OPEN_END: {
                    stream << " type=OPEN_END start=" << range.start;
                    break;
                }

                case RangeType::BOUNDED: {
                    stream << " type=BOUNDED start=" << range.start << " end=" << range.end;
                    break;
                }
            }
        }

        stream << ">";
        logger->info(stream.str());
    }
}