#ifndef CARPI_CARPI_WEBCLIENT_HPP
#define CARPI_CARPI_WEBCLIENT_HPP

#include <include/cef_client.h>
#include <X11/Xlib.h>

namespace carpi::ui {
    class WebClient : public CefClient/*, public CefRenderHandler*/ {
        IMPLEMENT_REFCOUNTING(WebClient);
        XDisplay* _display;

    public:
        explicit WebClient(XDisplay* display) : _display{display} {

        }

        /*CefRefPtr<CefRenderHandler> GetRenderHandler() override {
            return CefRefPtr<CefRenderHandler>(this);
        }*/

    };
}

#endif //CARPI_CARPI_WEBCLIENT_HPP
