#ifndef CARPI_NET_UTILS_HTTP_REQUEST_HPP
#define CARPI_NET_UTILS_HTTP_REQUEST_HPP

#include "url_parser.hpp"
#include <map>

namespace carpi::net {
    class HttpRequest {
        std::string _method;
        std::string _raw_url;
        Url _url;

        std::multimap<std::string, std::string> _headers;

        [[nodiscard]] bool has_header(const std::string& name) const;

    public:
        HttpRequest(std::string method, const std::string& url);

        [[nodiscard]] std::vector<uint8_t> send_data() const;

        HttpRequest& add_header(const std::string& key, const std::string& value) {
            _headers.emplace(key, value);
            return *this;
        }
    };
}

#endif //CARPI_NET_UTILS_HTTP_REQUEST_HPP
