#include <iostream>

#include <include/cef_app.h>
#include "main_app.hpp"
#include "gps_event_listener.hpp"

int main(int argc, char* argv[]) {
    CefMainArgs main_args{argc, argv};
    CefRefPtr<carpi::MainApp> app{new carpi::MainApp{}};
    sGpsEvtLstnr->run();
    return CefExecuteProcess(main_args, app, nullptr);
}