#ifndef CARPI_CARPI_LOCALSCHEMEHANDLER_HPP
#define CARPI_CARPI_LOCALSCHEMEHANDLER_HPP

#include <include/cef_scheme.h>
#include <common_utils/log.hpp>
#include <vector>

namespace carpi::io {
    class LocalSchemeHandler : public CefResourceHandler {
        IMPLEMENT_REFCOUNTING(LocalSchemeHandler);

        LOGGER;

        std::string _file_name;
        bool _is_found = false;
        bool _has_file_error = false;
        std::string _file_error;
        std::size_t _position = 0;
        std::size_t _file_size = 0;
        std::string _extension{};
        FILE* _fd = nullptr;

    public:
        ~LocalSchemeHandler() override;

        bool Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) override;

        void GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) override;

        bool Skip(int64 bytes_to_skip, int64 &bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) override;

        bool Read(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefResourceReadCallback> callback) override;

        void Cancel() override {

        }

        class Factory : public CefSchemeHandlerFactory {
            IMPLEMENT_REFCOUNTING(CefSchemeHandlerFactory);

        public:
            CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &scheme_name, CefRefPtr<CefRequest> request) override {
                return CefRefPtr<CefResourceHandler>(new LocalSchemeHandler{});
            }
        };
    };
}

#endif //CARPI_CARPI_LOCALSCHEMEHANDLER_HPP
