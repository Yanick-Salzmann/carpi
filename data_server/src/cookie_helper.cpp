#include <common_utils/string.hpp>
#include "cookie_helper.hpp"

namespace carpi::data {

    CookieHelper::CookieHelper(const std::multimap<std::string, std::string> &headers) {
        const auto rng = headers.equal_range("cookie");
        for(auto itr = rng.first; itr != rng.second; ++itr) {
            const auto cookie_val = itr->second;
            split_cookies(cookie_val);
        }
    }

    void CookieHelper::split_cookies(const std::string &line) {
        auto next_token = line.find(';');
        auto cur_position = 0;

        while(next_token != std::string::npos) {
            process_raw_cookie(utils::trim(line.substr(cur_position, next_token)));
            cur_position = next_token + 2; // '; ' -> 2 characters
            next_token = line.find(';', cur_position);
        }

        if(cur_position < line.size()) {
            const auto last_cookie = line.substr(cur_position);
            process_raw_cookie(last_cookie);
        }
    }

    void CookieHelper::process_raw_cookie(const std::string &cookie) {
        const auto separator = cookie.find('=');
        if(separator == std::string::npos) {
            _cookies.emplace(cookie, std::string{});
        } else {
            const auto key = utils::trim(cookie.substr(0, separator));
            const auto value = utils::trim(cookie.substr(separator + 1));
            _cookies.emplace(key, value);
        }
    }

    void CookieHelper::print(utils::Logger &logger) {
        for(const auto& itr : _cookies) {
            logger->info("{} = {}", itr.first, itr.second);
        }
    }
}