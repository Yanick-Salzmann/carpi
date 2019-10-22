#include "WebCore.hpp"

#include "main.hpp"

namespace carpi::ui {
    WebCore::WebCore() : _application(new WebApplication()) {
        CefMainArgs args{_argc, _argv};

    }
}