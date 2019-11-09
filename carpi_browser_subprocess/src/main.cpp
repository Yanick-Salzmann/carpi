#include <iostream>

#include <include/cef_app.h>
#include <include/wrapper/cef_message_router.h>

class MainApp : public CefApp, CefRenderProcessHandler {
    IMPLEMENT_REFCOUNTING(MainApp);

    CefRefPtr<CefMessageRouterRendererSide> _router;

public:
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
        registrar->AddCustomScheme("carpi", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
    }

    void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override {
        CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
    }

    void OnWebKitInitialized() override {
        CefMessageRouterConfig config;
        config.js_query_function = "sendCefRequest";
        _router = CefMessageRouterRendererSide::Create (config);
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override {
        _router->OnContextCreated (browser, frame, context);
    }

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override {
        _router->OnContextReleased (browser, frame, context);
    }

    void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override {
        CefRenderProcessHandler::OnFocusedNodeChanged(browser, frame, node);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override {
        return _router->OnProcessMessageReceived (browser, frame, source_process, message);
    }
};

int main(int argc, char* argv[]) {
    CefMainArgs main_args{argc, argv};
    CefRefPtr<MainApp> app{new MainApp{}};

    return CefExecuteProcess(main_args, app, nullptr);
}