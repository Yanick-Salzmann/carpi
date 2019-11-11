#ifndef CARPI_CARPI_WEBCORE_HPP
#define CARPI_CARPI_WEBCORE_HPP

#include "web_application.hpp"

#include <common_utils/log.hpp>
#include <X11/Xlib.h>

namespace carpi::ui {
    class WebCore {
        LOGGER;

        std::thread _cef_runner_thread;

        CefRefPtr<WebApplication> _application;

        XDisplay* _display = nullptr;
        bool _is_shut_down = false;

        void cef_run_callback();

    public:
        WebCore();

        ~WebCore();

        void manual_shutdown();
    };
}

#endif //CARPI_CARPI_WEBCORE_HPP
