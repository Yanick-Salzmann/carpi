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

    void HttpRequest::configure_client(CURL *curl, curl_slist** header_list) const {
        curl_easy_setopt(curl, CURLOPT_URL, _raw_url.c_str());

        const auto lower_method = utils::to_lower(_method);
        if(lower_method == "get") {
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        } else if(lower_method == "post") {
            curl_easy_setopt(curl, CURLOPT_PUT, 0L);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        } else if(lower_method == "put") {
            curl_easy_setopt(curl, CURLOPT_POST, 0L);
            curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        } else {
            throw std::runtime_error{"Only GET, POST and PUT are supported for http requests for now"};
        }

        if(_headers.empty()) {
            *header_list = nullptr;
            return;
        }

        *header_list = nullptr;
        for(auto itr = _headers.begin(); itr != _headers.end(); ++itr) {
            auto tmp = curl_slist_append(*header_list, format_header_for_curl(itr).c_str());
            if(tmp == nullptr) {
                curl_slist_free_all(*header_list);
                throw std::runtime_error{"Error appending to curl header list"};
            }

            *header_list = tmp;
        }

        if(!has_header("Host")) {
            auto tmp = curl_slist_append(*header_list, fmt::format("Host: {}", _url.host).c_str());
            if(tmp == nullptr) {
                curl_slist_free_all(*header_list);
                throw std::runtime_error{"Error appending to curl header list"};
            }

            *header_list = tmp;
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, *header_list);
    }

    std::string HttpRequest::format_header_for_curl(std::multimap<std::string, std::string>::const_iterator iterator) {
        const auto key = iterator->first;
        const auto value = iterator->second;
        if(value.empty()) {
            return fmt::format("{};", key);
        } else {
            return fmt::format("{}: {}", key, value);
        }
    }
}