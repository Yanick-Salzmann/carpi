#include "WebCore.hpp"

#include "main.hpp"

#include <include/cef_version.h>

#include <filesystem>

namespace carpi::ui {
    LOGGER_IMPL(WebCore);

    WebCore::WebCore() : _application(new WebApplication()) {
        _cef_runner_thread = std::thread{[this]() { cef_run_callback(); }};
    }

    void WebCore::cef_run_callback() {
        std::filesystem::path cur_dir{"."};
        cur_dir = canonical(absolute(cur_dir));

        const auto subprocess_path = canonical(absolute(cur_dir / ".." / "carpi_browser_subprocess" / "carpi_browser_subprocess"));
        const auto cache_path = canonical(absolute(cur_dir / "cache"));
        if (!exists(cache_path)) {
            create_directories(cache_path);
        }

        log->info("CEF subprocess: {}", subprocess_path.string());
        log->info("CEF cache: {}", cache_path.string());

        CefMainArgs args{_argc, _argv};

        CefSettings settings{};
        CefString(&settings.browser_subprocess_path) = subprocess_path.string();
        CefString(&settings.cache_path) = cache_path.string();
        settings.ignore_certificate_errors = 1;
        settings.windowless_rendering_enabled = 1;
        settings.log_severity = LOGSEVERITY_VERBOSE;
        CefString(&settings.locales_dir_path) = "/usr/local/bin/locales";

        const auto init_result = CefInitialize(args, settings, _application, nullptr);
        if (!init_result) {
            log->error("Error initializing CEF");
            throw std::runtime_error{"Error initializing CEF"};
        }

        log->info("CEF initialized @ version: {}", CEF_VERSION);

        CefRunMessageLoop();
    }
}