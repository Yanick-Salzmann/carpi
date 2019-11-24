#include "common_utils/string.hpp"

#include <algorithm>
#include <sstream>

namespace carpi::utils {

    auto trim_left(const std::string &value) -> std::string {
        const auto itr = std::find_if(value.begin(), value.end(), [](const auto& chr) { return !std::isspace(chr); });
        if(itr == value.end()) {
            return {};
        }

        return std::string{itr, value.end()};
    }

    auto trim_right(const std::string &value) -> std::string {
        const auto itr = std::find_if(value.rbegin(), value.rend(), [](const auto& chr) { return !std::isspace(chr); });
        if(itr == value.rend()) {
            return {};
        }

        return std::string{value.begin(), itr.base()};
    }

    auto to_lower(const std::string &value) -> std::string {
        std::string ret{};
        std::transform(value.begin(), value.end(), std::back_inserter(ret), [](const auto& chr) { return std::tolower(chr); });
        return ret;
    }

    auto split(const std::string &value, const char &delimiter) -> std::vector<std::string> {
        std::vector<std::string> ret{};
        std::stringstream sstream;
        sstream.str(value);

        std::string cur_line;
        while(std::getline(sstream, cur_line, delimiter)) {
            ret.emplace_back(cur_line);
        }

        return ret;
    }
}
