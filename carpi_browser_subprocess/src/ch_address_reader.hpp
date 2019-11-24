#ifndef CARPI_CH_ADDRESS_READER_HPP
#define CARPI_CH_ADDRESS_READER_HPP

#include <include/cef_v8.h>

namespace carpi {
    class ChAddressReader : public CefV8Handler {
        IMPLEMENT_REFCOUNTING(ChAddressReader);

    public:
        auto Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) -> bool override;
    };
}

#endif //CARPI_CH_ADDRESS_READER_HPP
