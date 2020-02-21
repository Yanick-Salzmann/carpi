#ifndef CARPI_NET_UTILS_URL_PARSER_HPP
#define CARPI_NET_UTILS_URL_PARSER_HPP

#include <string>
#include <common_utils/log.hpp>
#include <curl/curl.h>

namespace carpi::net {
    struct parsed_url {
        std::string scheme;
        std::string user;
        std::string password;
        std::string options;
        std::string host;
        int32_t port = -1;
        std::string path;
        std::string query;
        std::string fragment;
        std::string zone_id;
    };

    class url_parser {
        LOGGER;

        static std::string to_string(CURLU* url, CURLUPart part);
        static int32_t to_int32(CURLU* url, CURLUPart part);
    public:
        url_parser() = delete;

        static parsed_url parse(const std::string& url);
    };
}

#endif //CARPI_NET_UTILS_URL_PARSER_HPP
