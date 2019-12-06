#include <iostream>

#include <include/cef_app.h>
#include "main_app.hpp"
#include "gps_listener_thread.hpp"

int main(int argc, char* argv[]) {

    CefMainArgs main_args{argc, argv};
    CefRefPtr<carpi::MainApp> app{new carpi::MainApp{}};

    return CefExecuteProcess(main_args, app, nullptr);
}