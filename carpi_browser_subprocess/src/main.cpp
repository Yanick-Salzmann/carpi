#include <iostream>

#include <include/cef_app.h>

class MainApp : public CefApp {
    IMPLEMENT_REFCOUNTING(MainApp);

public:
    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
        //registrar->AddCustomScheme("local", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
    }
};

int main(int argc, char* argv[]) {
    CefMainArgs main_args{argc, argv};
    CefRefPtr<MainApp> app{new MainApp{}};

    return CefExecuteProcess(main_args, app.get(), nullptr);
}