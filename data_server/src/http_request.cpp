#include "data_server/http_request.hpp"
#include "data_server/http_response.hpp"

#include <filesystem>

namespace carpi::data {
    LOGGER_IMPL(HttpRequest);

    HttpRequest::HttpRequest(const std::string &method, const std::string &path, const std::string &version, int socket) {
        if (method != "GET") {
            log->warn("Invalid HTTP method {}, must be GET", method);
            HttpResponse{HttpStatusCode::INVALID_METHOD, "GET only"}.write_to_socket(socket);
            return;
        }

        if (version != "HTTP/1.1") {
            log->warn("Invalid HTTP version {}, must be 'HTTP/1.1'", version);
            HttpResponse{HttpStatusCode::NOT_SUPPORTED, "Only HTTP/1.1 is supported"}.write_to_socket(socket);
            return;
        }

        if (path.empty()) {
            log->warn("HTTP path is empty");
            HttpResponse{HttpStatusCode::NOT_FOUND, "File not found"}.write_to_socket(socket);
            return;
        }

        auto file_path = path;
        if (file_path[0] == '/') {
            file_path = file_path.substr(1);
        }

        std::filesystem::path ui_root{"../../carpi_master/ui"};
        std::filesystem::path ui_file = canonical(absolute(ui_root / file_path));
        if (!exists(ui_file)) {
            log->warn("HTTP file not found: {}", ui_file.string());
            HttpResponse{HttpStatusCode::NOT_FOUND, "File not found"}.write_to_socket(socket);
            return;
        }

        HttpResponse{HttpStatusCode::OK, "OK"}.respond_with_file(ui_file.string()).write_to_socket(socket);
    }
}