#include "common_utils/string.hpp"

#include <algorithm>

namespace carpi::utils {

    std::string trim_left(const std::string &value) {
        const auto itr = std::find_if(value.begin(), value.end(), [](const auto& chr) { return !std::isspace(chr); });
        if(itr == value.end()) {
            return {};
        }

        return std::string{itr, value.end()};
    }

    std::string trim_right(const std::string &value) {
        const auto itr = std::find_if(value.rbegin(), value.rend(), [](const auto& chr) { return !std::isspace(chr); });
        if(itr == value.rend()) {
            return {};
        }

        return std::string{value.begin(), itr.base()};
    }

    std::string to_lower(const std::string &value) {
        std::string ret{};
        std::transform(value.begin(), value.end(), std::back_inserter(ret), [](const auto& chr) { return std::tolower(chr); });
        return ret;
    }
}
