#include <io/camera/camera_stream.hpp>
#include "WebClient.hpp"
#include <nlohmann/json.hpp>

namespace carpi::ui {
    LOGGER_IMPL(WebClient);

    bool WebClient::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString &request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
        using nlohmann::json;

        log->info("Received query named {} (persistent={})", request.ToString(), persistent);
        if(request == "camera_parameters") {
            uint32_t width, height, fps;
            sCameraStream->camera_parameters(width, height, fps);
            json val{
                    {"width", width},
                    {"height", height}
            };

            callback->Success(val.dump());
            return true;
        }

        return false;
    }
}