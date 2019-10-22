#ifndef CARPI_CARPI_WEBCORE_HPP
#define CARPI_CARPI_WEBCORE_HPP

#include "WebApplication.hpp"

namespace carpi::ui {
    class WebCore {
        CefRefPtr<WebApplication> _application;

    public:
        WebCore();
    };
}

#endif //CARPI_CARPI_WEBCORE_HPP
