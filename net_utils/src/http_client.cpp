#include "net_utils/http_client.hpp"
#include <mutex>
#include <algorithm>
#include <vector>
#include <cstring>
#include <common_utils/string.hpp>

namespace carpi::net {
    static std::once_flag _curl_once_flag;
    LOGGER_IMPL(HttpClient);

    struct HttpClientState {
        std::string status_line{};
        std::multimap<std::string, std::string> response_headers{};
        std::vector<uint8_t> response{};
        std::vector<uint8_t> upload{};
        std::size_t upload_progress;
    };

    namespace detail {
        std::size_t write_callback(char *data, std::size_t size, std::size_t elements, void *ptr) {
            auto state = (HttpClientState *) ptr;
            state->response.insert(state->response.end(), (uint8_t *) data, ((uint8_t *) data) + elements * size);
            return elements * size;
        }

        std::size_t read_callback(char *buffer, std::size_t size, std::size_t elements, void *ptr) {
            auto state = (HttpClientState *) ptr;
            if (state->upload_progress >= state->upload.size()) {
                return 0;
            }

            const auto to_read = std::min<std::size_t>(size * elements, state->upload.size() - state->upload_progress);
            if (!to_read) {
                return 0;
            }

            memcpy(buffer, &state->upload[state->upload_progress], to_read);
            state->upload_progress += to_read;
            return to_read;
        }

        std::size_t header_callback(char *data, std::size_t size, std::size_t elements, void *ptr) {
            auto state = (HttpClientState *) ptr;
            std::string header_line{data, data + (size * elements)};

            if(utils::trim(header_line).empty()) {
                return elements * size;
            }

            if (header_line.find("HTTP/1.1") == 0) {
                state->response_headers.clear();
                state->status_line = utils::trim(header_line);
                return size * elements;
            }

            const auto idx_colon = header_line.find(':');
            if (idx_colon == std::string::npos) {
                state->response_headers.emplace(utils::trim(header_line), "");
            } else {
                state->response_headers.emplace(utils::trim(header_line.substr(0, idx_colon)), utils::trim(header_line.substr(idx_colon + 1)));
            }

            return size * elements;
        }
    }

    HttpClient::HttpClient() {
        std::call_once(_curl_once_flag, []() {
            curl_global_init(CURL_GLOBAL_ALL);
        });

        pthread_mutex_init(&_request_lock, nullptr);

        _curl = curl_easy_init();

        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, detail::write_callback);
        curl_easy_setopt(_curl, CURLOPT_READFUNCTION, detail::read_callback);
        curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, detail::header_callback);
        curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, _curl_error_buffer);
    }

    HttpClient::~HttpClient() {
        curl_easy_cleanup(_curl);
    }

    HttpResponse HttpClient::execute(const HttpRequest &request) {
        pthread_mutex_lock(&_request_lock);
        curl_slist *header_list = nullptr;
        try {
            request.configure_client(_curl, &header_list);
        } catch (std::exception &e) {
            pthread_mutex_unlock(&_request_lock);
            throw e;
        }

        HttpClientState state{
                .upload = request.body(),
                .upload_progress = 0
        };

        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &state);
        curl_easy_setopt(_curl, CURLOPT_READDATA, &state);
        curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &state);

        const auto rc = curl_easy_perform(_curl);
        curl_slist_free_all(header_list);
        pthread_mutex_unlock(&_request_lock);

        if (rc != CURLE_OK) {
            _curl_error_buffer[std::size(_curl_error_buffer) - 1] = '\0';
            log->error("Error executing request: {}", (const char *) _curl_error_buffer);
            throw std::runtime_error{"Error executing HTTP request"};
        }

        return {state.status_line, state.response_headers, state.response};
    }
}