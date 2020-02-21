#ifndef CARPI_NET_UTILS_HTTP_RESPONSE_HPP
#define CARPI_NET_UTILS_HTTP_RESPONSE_HPP

#include <string>
#include <map>
#include <vector>

namespace carpi::net {
    class http_response {
        std::string _status_line;
        std::multimap<std::string, std::string> _headers;
        std::vector<uint8_t> _body;

        std::string _protocol;
        uint32_t _status_code;
        std::string _status_text;

    public:
        http_response(std::string status_line, std::multimap<std::string, std::string> headers, std::vector<uint8_t> body);

        [[nodiscard]] std::string header(const std::string& key) const {
            return _headers.lower_bound(key)->second;
        }

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] const std::vector<uint8_t>& body() const {
            return _body;
        }

        uint32_t status_code() const {
            return _status_code;
        }

        std::string status_text() const {
            return _status_text;
        }
    };
}

#endif //CARPI_NET_UTILS_HTTP_RESPONSE_HPP
