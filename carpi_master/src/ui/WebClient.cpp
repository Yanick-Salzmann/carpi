#include "WebClient.hpp"

namespace carpi::ui {
    LOGGER_IMPL(WebClient);

    bool WebClient::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString &request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
        log->info("Received query named {} (persistent={})", request.ToString(), persistent);
        return false;
    }
}