#ifndef CARPI_CARPI_WEBAPPLICATION_HPP
#define CARPI_CARPI_WEBAPPLICATION_HPP

#include <include/cef_app.h>

namespace carpi::ui {
    class WebApplication : public CefApp {
        IMPLEMENT_REFCOUNTING(WebApplication);

    public:
        void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
            registrar->AddCustomScheme("carpi", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
        }
    };
}

#endif //CARPI_CARPI_WEBAPPLICATION_HPP
