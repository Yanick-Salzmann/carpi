#ifndef CARPI_COMMON_UTILS_STRING_HPP
#define CARPI_COMMON_UTILS_STRING_HPP

#include <string>
#include <vector>

namespace carpi::utils {
    auto trim_left(const std::string& value) -> std::string;
    auto trim_right(const std::string& value) -> std::string;

    inline auto trim(const std::string& value) -> std::string {
        return trim_left(trim_right(value));
    }

    auto to_lower(const std::string& value) -> std::string;

    auto split(const std::string& value, const char& delimiter) -> std::vector<std::string>;
    void split(const std::string& value, const char& delimiter, std::vector<std::string>& parts);

    std::string replace_all(const std::string& str, const char& chr, const char& replace);
}

#endif //CARPI_COMMON_UTILS_STRING_HPP
