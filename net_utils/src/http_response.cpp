#include <spdlog/spdlog.h>
#include "net_utils/http_response.hpp"
#include <sstream>

namespace carpi::net {
    http_response::http_response(std::string status_line, std::multimap<std::string, std::string> headers, std::vector<uint8_t> body) :
            _status_line{std::move(status_line)},
            _headers{std::move(headers)},
            _body{std::move(body)} {
        const auto proto_end = _status_line.find(' ');
        if(proto_end == std::string::npos) {
            throw std::runtime_error{"Invalid http status line, could not find end of protocol"};
        }

        _protocol = _status_line.substr(0, proto_end);

        const auto status_end = _status_line.find(' ', proto_end + 1);
        if(status_end == std::string::npos) {
            throw std::runtime_error{"Invalid http status line, could not find end of status code"};
        }

        _status_code = (uint32_t) std::stoul(_status_line.substr(proto_end + 1, (status_end - proto_end) - 1));
        _status_text = _status_line.substr(status_end + 1);
    }

    std::string http_response::to_string() const {
        std::stringstream header_stream;
        header_stream << "[";

        auto first = true;
        for(const auto& header : _headers) {
            if(first) {
                first = false;
            } else {
                header_stream << ", ";
            }

            header_stream << "{ " << header.first << ": " << header.second << " }";
        }

        header_stream << "]";
        return fmt::format("<HttpResponse protocol='{}', status_code={}, status_text='{}', headers='{}', body='{} bytes'>", _protocol, _status_code, _status_text, header_stream.str(), _body.size());
    }
}