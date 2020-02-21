#include "net_utils/url_parser.hpp"

namespace carpi::net {
    LOGGER_IMPL(url_parser);

    parsed_url url_parser::parse(const std::string &address) {
        const auto url = curl_url();
        if (url == nullptr) {
            log->error("Error allocating CURL url parser instance");
            throw std::runtime_error{"Error allocating CURL url parser"};
        }

        std::shared_ptr<CURLU> ptr{url, [](CURLU* curlu) { curl_url_cleanup(curlu); }};

        auto error_code = curl_url_set(url, CURLUPART_URL, address.c_str(), CURLU_NON_SUPPORT_SCHEME);
        if(error_code) {
            log->error("Error parsing URL '{}': {} (TODO: error code to string)", address, (std::size_t) error_code);
            throw std::runtime_error{"Error parsing URL"};
        }

        return {
          .scheme = to_string(url, CURLUPART_SCHEME),
          .user = to_string(url, CURLUPART_USER),
          .password = to_string(url, CURLUPART_PASSWORD),
          .options = to_string(url, CURLUPART_OPTIONS),
          .host = to_string(url, CURLUPART_HOST),
          .port = to_int32(url, CURLUPART_PORT),
          .path = to_string(url, CURLUPART_PATH),
          .query = to_string(url, CURLUPART_QUERY),
          .fragment = to_string(url, CURLUPART_FRAGMENT),
#if LIBCURL_VERSION_MAJOR > 7 && LIBCURL_VERSION_MINOR >= 68
          .zone_id = to_string(url, CURLUPART_ZONEID)
#endif
        };
    }

    std::string url_parser::to_string(CURLU *url, CURLUPart part) {
        char* url_part = nullptr;
        const auto error_code = curl_url_get(url, part, &url_part, 0);
        if(error_code || !url_part) {
            return {};
        }

        std::string content = url_part;
        curl_free(url_part);
        return content;
    }

    int32_t url_parser::to_int32(CURLU *url, CURLUPart part) {
        const auto str_value = to_string(url, part);
        if(str_value.empty()) {
            return -1;
        }

        return std::stoi(str_value);
    }
}