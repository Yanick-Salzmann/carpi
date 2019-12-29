#include <net_utils/http_request.hpp>
#include <sstream>
#include <common_utils/string.hpp>
#include <common_utils/conversion.hpp>

namespace carpi::net {
    HttpRequest::HttpRequest(std::string method, const std::string &url) : _method(std::move(method)), _url(UrlParser::parse(url)), _raw_url(url) {

    }

    std::vector<uint8_t> HttpRequest::send_data() const {


        std::stringstream stream;
        stream << _method << " " << _raw_url << " HTTP/1.1\r\n";
        if(!has_header("Host")) {
            stream << "Host: " << _url.host << "\r\n";
        }

        for(const auto& pair : _headers) {
            stream << pair.first << ": " << pair.second << "\r\n";
        }

        stream << "\r\n";

        return utils::utf8_to_bytes(stream.str());
    }

    bool HttpRequest::has_header(const std::string &name) const {
        const auto name_lower = utils::to_lower(name);
        for(const auto& pair : _headers) {
            auto key_lower = utils::to_lower(pair.first);
            if(key_lower == name_lower) {
                return true;
            }
        }

        return false;
    }
}