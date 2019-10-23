#ifndef CARPI_CARPI_WEBCORE_HPP
#define CARPI_CARPI_WEBCORE_HPP

#include "WebApplication.hpp"

#include <common_utils/log.hpp>
#include <X11/Xlib.h>

namespace carpi::ui {
    class WebCore {
        LOGGER;

        std::thread _cef_runner_thread;

        CefRefPtr<WebApplication> _application;

        XDisplay* _display = nullptr;

        void cef_run_callback();

    public:
        WebCore();

        ~WebCore();
    };
}

#endif //CARPI_CARPI_WEBCORE_HPP
