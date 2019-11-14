#include "web_core.hpp"

#include "main.hpp"
#include "web_client.hpp"

#include <include/cef_version.h>
#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>

#include <filesystem>
#include <io/local_scheme_handler.hpp>
#include <io/camera/camera_stream.hpp>
#include <ui/events/bluetooth_events.hpp>

namespace carpi::ui {
    LOGGER_IMPL(WebCore);

    WebCore::WebCore() : _application(new WebApplication()) {
        _cef_runner_thread = std::thread{[this]() { cef_run_callback(); }};
    }

    WebCore::~WebCore() {
        if (_display != nullptr) {
            XCloseDisplay(_display);
            _display = nullptr;
        }

        manual_shutdown();
    }

    void WebCore::cef_run_callback() {
        std::filesystem::path cur_dir{"."};
        cur_dir = canonical(absolute(cur_dir));

        const auto subprocess_path = canonical(absolute(cur_dir / ".." / "carpi_browser_subprocess" / "carpi_browser_subprocess"));
        const auto cache_path = canonical(absolute(cur_dir / "cache"));
        if (!exists(cache_path)) {
            create_directories(cache_path);
        }

        _display = XOpenDisplay(nullptr);
        const auto screen = DefaultScreenOfDisplay(_display);
        log->info("Screen size: {}x{}", screen->width, screen->height);

        log->info("CEF subprocess: {}", subprocess_path.string());
        log->info("CEF cache: {}", cache_path.string());

        CefMainArgs args{_argc, _argv};

        CefSettings settings{};
        CefString(&settings.browser_subprocess_path) = subprocess_path.string();
        CefString(&settings.cache_path) = cache_path.string();
        settings.ignore_certificate_errors = 1;
        settings.windowless_rendering_enabled = 1;
        settings.log_severity = LOGSEVERITY_DEFAULT;
        settings.no_sandbox = 1;
        settings.remote_debugging_port = 52100;
        CefString(&settings.locales_dir_path) = "/usr/local/bin/locales";

        const auto init_result = CefInitialize(args, settings, _application, nullptr);
        if (!init_result) {
            log->error("Error initializing CEF");
            throw std::runtime_error{"Error initializing CEF"};
        }

        log->info("CEF initialized @ version: {}", CEF_VERSION);
        log->info("CEF version:               {}.{}.{}", CEF_VERSION_MAJOR, CEF_VERSION_MINOR, CEF_COMMIT_NUMBER);
        log->info("Chromium version:          {}.{}.{}", CHROME_VERSION_MAJOR, CHROME_VERSION_MINOR, CHROME_VERSION_BUILD);

        post_initialize();

        CefWindowInfo window_info{};
        CefBrowserSettings browser_settings{};

        browser_settings.universal_access_from_file_urls = STATE_ENABLED;
        browser_settings.file_access_from_file_urls = STATE_ENABLED;
        browser_settings.windowless_frame_rate = 60; // just in case

        window_info.x = window_info.y = 0;
        window_info.width = screen->width;
        window_info.height = screen->height;
        window_info.windowless_rendering_enabled = 0;

        CefBrowserHost::CreateBrowser(window_info, CefRefPtr<WebClient>(new WebClient()), "", browser_settings, nullptr, nullptr);

        CefRunMessageLoop();
    }

    void WebCore::manual_shutdown() {
        if(_is_shut_down) {
            return;
        }

        _is_shut_down = true;

        CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefQuitMessageLoop)));

        if (_cef_runner_thread.joinable()) {
            _cef_runner_thread.join();
        }
    }

    void WebCore::post_initialize() {
        CefRegisterSchemeHandlerFactory("carpi", "", CefRefPtr<CefSchemeHandlerFactory>{new io::LocalSchemeHandler::Factory{}});
        sCameraStream->init_events();
        sBluetoothEvents->init_events();
    }
}