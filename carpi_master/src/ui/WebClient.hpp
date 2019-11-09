#ifndef CARPI_CARPI_WEBCLIENT_HPP
#define CARPI_CARPI_WEBCLIENT_HPP

#include <include/cef_client.h>
#include <X11/Xlib.h>
#undef Success
#include <include/wrapper/cef_message_router.h>

namespace carpi::ui {
    class WebClient : public CefClient, public CefLifeSpanHandler, public CefMessageRouterBrowserSide::Handler {
        IMPLEMENT_REFCOUNTING(WebClient);

        CefRefPtr<CefMessageRouterBrowserSide> _message_router;
        CefRefPtr<CefBrowser> _browser;

    public:
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
            return this;
        }

        void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
            _browser = browser;

            auto settings = CefMessageRouterConfig ();
            settings.js_query_function = "sendCefRequest";

            _message_router = CefMessageRouterBrowserSide::Create (settings);
            _message_router->AddHandler (this, true);

            browser->GetMainFrame()->LoadURL(CefString("carpi://localhost/main.html"));
        }

        void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
            _message_router->RemoveHandler (this);
            _message_router->OnBeforeClose (_browser);

            _message_router = nullptr;
        }

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override {
            return _message_router->OnProcessMessageReceived (browser, frame, source_process, message);
        }

        bool OnQuery (CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) override;
    };
}

#endif //CARPI_CARPI_WEBCLIENT_HPP
