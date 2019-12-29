#ifndef CARPI_NET_UTILS_HTTP_REQUEST_HPP
#define CARPI_NET_UTILS_HTTP_REQUEST_HPP

#include "url_parser.hpp"
#include <map>
#include <curl/curl.h>

namespace carpi::net {
    class HttpRequest {
        typedef std::multimap<std::string, std::string> header_map;

        std::string _method;
        std::string _raw_url;
        Url _url;

        std::vector<uint8_t> _body{};

        header_map _headers;

        [[nodiscard]] bool has_header(const std::string& name) const;

        static std::string format_header_for_curl(header_map::const_iterator iterator);

    public:
        HttpRequest(std::string method, const std::string& url);

        [[nodiscard]] std::vector<uint8_t> send_data() const;

        HttpRequest& add_header(const std::string& key, const std::string& value) {
            _headers.emplace(key, value);
            return *this;
        }

        void configure_client(CURL* curl, curl_slist** header_list) const;

        const std::vector<uint8_t>& body() const {
            return _body;
        }
    };
}

#endif //CARPI_NET_UTILS_HTTP_REQUEST_HPP
