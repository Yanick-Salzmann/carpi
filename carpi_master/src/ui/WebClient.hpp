#ifndef CARPI_CARPI_WEBCLIENT_HPP
#define CARPI_CARPI_WEBCLIENT_HPP

#include <include/cef_client.h>
#include <X11/Xlib.h>

namespace carpi::ui {
class WebClient : public CefClient {
        IMPLEMENT_REFCOUNTING(WebClient);
    public:

    };
}

#endif //CARPI_CARPI_WEBCLIENT_HPP
