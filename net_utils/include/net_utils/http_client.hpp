#ifndef CARPI_NET_UTILS_HTTP_CLIENT_HPP
#define CARPI_NET_UTILS_HTTP_CLIENT_HPP

#include <curl/curl.h>
#include <pthread.h>
#include "http_request.hpp"
#include "http_response.hpp"
#include <common_utils/log.hpp>

namespace carpi::net {
    class http_client {
        LOGGER;

        pthread_mutex_t _request_lock;
        char _curl_error_buffer[CURL_ERROR_SIZE + 1]{};

        CURL* _curl;

    public:
        http_client();
        ~http_client();

        http_response execute(const http_request& request);
    };
}

#endif //CARPI_NET_UTILS_HTTP_CLIENT_HPP
