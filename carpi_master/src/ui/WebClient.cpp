#include "WebClient.hpp"

namespace carpi::ui {

    bool WebClient::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString &request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
        return false;
    }
}