#ifndef CARPI_CARPI_SUBPROC_ENV_VAR_CALLBACK_HPP
#define CARPI_CARPI_SUBPROC_ENV_VAR_CALLBACK_HPP

#include <include/cef_app.h>

namespace carpi {
    class EnvVarCallback : public CefV8Handler {
        IMPLEMENT_REFCOUNTING(EnvVarCallback);

    public:
        bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override;
    };
}

#endif //CARPI_CARPI_SUBPROC_ENV_VAR_CALLBACK_HPP
