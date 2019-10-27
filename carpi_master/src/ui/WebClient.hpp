#ifndef CARPI_CARPI_WEBCLIENT_HPP
#define CARPI_CARPI_WEBCLIENT_HPP

#include <include/cef_client.h>
#include <X11/Xlib.h>

namespace carpi::ui {
class WebClient : public CefClient, public CefLifeSpanHandler {
        IMPLEMENT_REFCOUNTING(WebClient);
    public:
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        browser->GetMainFrame()->LoadURL(CefString("local://ui/test.html"));
    }
};
}

#endif //CARPI_CARPI_WEBCLIENT_HPP
