#include "WebCore.hpp"

#include "main.hpp"
#include "WebClient.hpp"

#include <include/cef_version.h>

#include <filesystem>
#include <io/LocalSchemeHandler.hpp>

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
        settings.windowless_rendering_enabled = 0;
        settings.log_severity = LOGSEVERITY_VERBOSE;
        CefString(&settings.locales_dir_path) = "/usr/local/bin/locales";

        const auto init_result = CefInitialize(args, settings, _application, nullptr);
        if (!init_result) {
            log->error("Error initializing CEF");
            throw std::runtime_error{"Error initializing CEF"};
        }

        log->info("CEF initialized @ version: {}", CEF_VERSION);

        CefRegisterSchemeHandlerFactory("local", CefString{}, CefRefPtr<CefSchemeHandlerFactory>{new io::LocalSchemeHandler::Factory{}});

        CefWindowInfo window_info{};
        CefBrowserSettings browser_settings{};

        window_info.x = window_info.y = 0;
        window_info.width = screen->width;
        window_info.height = screen->height;

        CefBrowserHost::CreateBrowser(window_info, CefRefPtr<WebClient>(new WebClient()), CefString("local://ui/test.html"), browser_settings, nullptr, nullptr);

        CefRunMessageLoop();
    }
}