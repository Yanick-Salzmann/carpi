#ifndef CARPI_CARPI_GPS_CALLBACK_HPP
#define CARPI_CARPI_GPS_CALLBACK_HPP

#include <include/cef_v8.h>

namespace carpi {
    class GpsCallback : public CefV8Handler {
    public:
        bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override;
    };
}

#endif //CARPI_CARPI_GPS_CALLBACK_HPP
