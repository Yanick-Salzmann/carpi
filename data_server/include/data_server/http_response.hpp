#ifndef CARPI_DATA_SERVER_HTTP_RESPONSE_HPP
#define CARPI_DATA_SERVER_HTTP_RESPONSE_HPP

#include <map>
#include <string>

namespace carpi::data {
    enum class HttpStatusCode {
        OK = 200,
        NOT_FOUND = 404,
        BAD_REQUEST = 400,
        NOT_AUTHORIZED = 401,
        FORBIDDEN = 403,
        INVALID_METHOD = 405,
        INTERNAL_SERVER_ERROR = 500,
        NOT_SUPPORTED = 505
    };

    class HttpResponse {
        std::multimap<std::string, std::string> _headers;

        int32_t _status_code = (int32_t) HttpStatusCode::OK;
        std::string _status_text = "OK";

        FILE* _response_file = nullptr;

    public:
        HttpResponse(HttpStatusCode status, std::string status_text);
        HttpResponse(int32_t status, std::string status_text);

        ~HttpResponse();

        void add_header(const std::string& name, const std::string& value);

        HttpResponse& respond_with_file(const std::string& path);

        void write_to_socket(int socket);
    };
}

#endif //CARPI_DATA_SERVER_HTTP_RESPONSE_HPP
