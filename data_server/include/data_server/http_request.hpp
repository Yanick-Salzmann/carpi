#ifndef CARPI_DATA_SERVER_HTTP_REQUEST_HPP
#define CARPI_DATA_SERVER_HTTP_REQUEST_HPP

#include <string>
#include "http_response.hpp"
#include <common_utils/log.hpp>

namespace carpi::data {
    class HttpRequest {
        LOGGER;

    public:
        HttpRequest(const std::string& method, const std::string& path, const std::string& version, int socket);
    };
}

#endif //CARPI_DATA_SERVER_HTTP_REQUEST_HPP
