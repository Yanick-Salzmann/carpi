#ifndef CARPI_DATA_SERVER_HTTP_REQUEST_HPP
#define CARPI_DATA_SERVER_HTTP_REQUEST_HPP

#include <string>
#include "http_response.hpp"
#include <common_utils/log.hpp>

namespace carpi::data {
    enum class RequestType {
        LOCAL_FILE,
        CAMERA_STREAM,
        UNKNOWN
    };

    class HttpRequest {
        LOGGER;

        static RequestType find_request_type(const std::string& path, std::string& remainder);
        static void process_local_file(const std::string& path, int socket);
        static void process_camera_stream(const std::string& path, const std::multimap<std::string, std::string>& headers, int socket);

        static std::string create_uuid();

    public:
        HttpRequest(const std::string& method, const std::string& path, const std::string& version, const std::multimap<std::string, std::string>& headers, int socket);
    };
}

#endif //CARPI_DATA_SERVER_HTTP_REQUEST_HPP
