#ifndef CARPI_DATA_SERVER_HTTP_RESPONSE_HPP
#define CARPI_DATA_SERVER_HTTP_RESPONSE_HPP

#include <map>
#include <string>

#include <common_utils/log.hpp>

namespace carpi::data {
    enum class HttpStatusCode {
        OK = 200,
        PARTIAL_CONTENT = 206,
        NOT_FOUND = 404,
        BAD_REQUEST = 400,
        NOT_AUTHORIZED = 401,
        FORBIDDEN = 403,
        INVALID_METHOD = 405,
        RANGE_NOT_SATISFIABLE = 416,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        NOT_SUPPORTED = 505
    };

    class HttpResponse {
        LOGGER;

        std::multimap<std::string, std::string> _headers;

        int32_t _status_code = (int32_t) HttpStatusCode::OK;
        std::string _status_text = "OK";

        FILE* _response_file = nullptr;

    public:
        HttpResponse(HttpStatusCode status, std::string status_text);
        HttpResponse(int32_t status, std::string status_text);

        ~HttpResponse();

        HttpResponse& add_header(const std::string& name, const std::string& value);

        HttpResponse& respond_with_file(const std::string& path);

        HttpResponse& status(HttpStatusCode code, std::string text = "") {
            _status_code = (int32_t) code;
            _status_text = std::move(text);
            return *this;
        }

        void write_to_socket(int socket);
    };
}

#endif //CARPI_DATA_SERVER_HTTP_RESPONSE_HPP
