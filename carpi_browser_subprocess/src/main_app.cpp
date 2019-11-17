#include "main_app.hpp"
#include "camera_frame_callback.hpp"
#include "env_var_callback.hpp"
#include "gps_event_listener.hpp"

namespace carpi {

    CefRefPtr<CefRenderProcessHandler> MainApp::GetRenderProcessHandler() {
        return this;
    }

    void MainApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
        registrar->AddCustomScheme("carpi", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
    }

    void MainApp::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) {
        CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
    }

    void MainApp::OnWebKitInitialized() {
        CefMessageRouterConfig config;
        config.js_query_function = "sendCefRequest";
        _router = CefMessageRouterRendererSide::Create(config);
    }

    void MainApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
        _router->OnContextCreated(browser, frame, context);

        context->GetGlobal()->SetValue("fetch_raw_frame", CefV8Value::CreateFunction("fetch_raw_frame", new carpi::CameraFrameCallback()), V8_PROPERTY_ATTRIBUTE_READONLY);
        context->GetGlobal()->SetValue("get_env_value", CefV8Value::CreateFunction("get_env_value", new carpi::EnvVarCallback()), V8_PROPERTY_ATTRIBUTE_READONLY);
    }

    void MainApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
        _router->OnContextReleased(browser, frame, context);
    }

    void MainApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) {
        CefRenderProcessHandler::OnFocusedNodeChanged(browser, frame, node);
    }

    bool MainApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
        return _router->OnProcessMessageReceived(browser, frame, source_process, message);
    }

    void MainApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info) {
        CefRenderProcessHandler::OnBrowserCreated(browser, extra_info);

        sGpsEvtLstnr->run();
    }
}