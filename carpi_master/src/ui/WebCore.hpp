#ifndef CARPI_CARPI_WEBCORE_HPP
#define CARPI_CARPI_WEBCORE_HPP

#include "WebApplication.hpp"

#include <common_utils/log.hpp>

namespace carpi::ui {
    class WebCore {
        LOGGER;

        CefRefPtr<WebApplication> _application;

    public:
        WebCore();
    };
}

#endif //CARPI_CARPI_WEBCORE_HPP
