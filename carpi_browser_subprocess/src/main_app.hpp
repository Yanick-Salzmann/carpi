#ifndef CARPI_CARPI_SUBPROC_MAIN_APP_HPP
#define CARPI_CARPI_SUBPROC_MAIN_APP_HPP

#include <include/cef_app.h>
#include <include/wrapper/cef_message_router.h>

namespace carpi {
    class MainApp : public CefApp, CefRenderProcessHandler {
        IMPLEMENT_REFCOUNTING(MainApp);

        CefRefPtr<CefMessageRouterRendererSide> _router;

    public:
        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

        void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

        void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override;

        void OnWebKitInitialized() override;

        void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

        void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

        void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override;

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
    };
}

#endif //CARPI_CARPI_SUBPROC_MAIN_APP_HPP
