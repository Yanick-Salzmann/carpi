#ifndef CARPI_DATA_SERVER_COOKIE_HELPER_HPP
#define CARPI_DATA_SERVER_COOKIE_HELPER_HPP

#include <map>
#include <common_utils/log.hpp>

namespace carpi::data {
    class CookieHelper {
        std::map<std::string, std::string> _cookies;

        void process_raw_cookie(const std::string& raw_cookie);
        void split_cookies(const std::string& line);

    public:
        explicit CookieHelper(const std::multimap<std::string, std::string>& headers);

        [[nodiscard]] bool has_cookies() const {
            return !_cookies.empty();
        }

        [[nodiscard]] bool has_cookie(const std::string& key) const {
            return _cookies.find(key) != _cookies.end();
        }

        [[nodiscard]] std::string cookie(const std::string& key) const {
            const auto itr = _cookies.find(key);
            return (itr != _cookies.end()) ? itr->second : std::string{};
        }

        void print(utils::Logger& logger);
    };
}

#endif //CARPI_DATA_SERVER_COOKIE_HELPER_HPP
