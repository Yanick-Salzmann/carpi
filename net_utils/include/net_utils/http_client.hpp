#ifndef CARPI_NET_UTILS_HTTP_CLIENT_HPP
#define CARPI_NET_UTILS_HTTP_CLIENT_HPP

#include <curl/curl.h>
#include <pthread.h>
#include "http_request.hpp"
#include "http_response.hpp"
#include <common_utils/log.hpp>

namespace carpi::net {
    class HttpClient {
        LOGGER;

        pthread_mutex_t _request_lock;
        char _curl_error_buffer[CURL_ERROR_SIZE + 1]{};

        CURL* _curl;

    public:
        HttpClient();
        ~HttpClient();

        HttpResponse execute(const HttpRequest& request);
    };
}

#endif //CARPI_NET_UTILS_HTTP_CLIENT_HPP
