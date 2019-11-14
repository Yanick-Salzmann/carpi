#include <io/camera/camera_stream.hpp>
#include "web_client.hpp"
#include "event_manager.hpp"
#include <nlohmann/json.hpp>

namespace carpi::ui {
    LOGGER_IMPL(WebClient);

    bool WebClient::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString &request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
        if (request == "EventHandlerInit") {
            sUiEventMgr->apply_event_callback([callback](const std::string& payload) {
               callback->Success(payload);
            });
            return true;
        }

        using nlohmann::json;

        json req_obj = json::parse(request.ToString());
        const std::string type = req_obj["type"];
        const std::string payload = (req_obj.find("request") != req_obj.end()) ? std::string{req_obj["request"]} : "{}";

        const auto response = sUiEventMgr->handle_event(type, payload);
        if(response.empty()) {
            callback->Success(json{{"type", type}, {"body", {}}}.dump());
        } else {
            callback->Success(json{{"type", type}, {"body", response}}.dump());
        }

        return true;
    }
}