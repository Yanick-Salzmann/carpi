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

        void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override {
            command_line->AppendSwitchWithValue("--disable-features", "CrossSiteDocumentBlockingAlways,CrossSiteDocumentBlockingIfIsolating");
            CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
        }
    };
}

#endif //CARPI_CARPI_WEBAPPLICATION_HPP
