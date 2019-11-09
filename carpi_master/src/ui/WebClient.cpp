#include <io/camera/camera_stream.hpp>
#include "WebClient.hpp"
#include <nlohmann/json.hpp>

namespace carpi::ui {
    LOGGER_IMPL(WebClient);

    bool WebClient::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString &request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
        if (request == "EventHandlerInit") {
            // TODO: Register handler
            return true;
        }

        using nlohmann::json;

        json req_obj = json::parse(request.ToString());
        const std::string type = req_obj["type"];

        if (type == "camera_parameters") {
            uint32_t width, height, fps;
            sCameraStream->camera_parameters(width, height, fps);
            json val{
                    {"type", type},
                    {"body", {
                                     {"width", width},
                                     {"height", height}
                             }
                    }
            };

            callback->Success(val.dump());
            return true;
        } else if(type == "camera_frame") {
            uint32_t w, h;
            const auto b64 = sCameraStream->buffer_to_base64(w, h);
            json val{
                    {"type", type},
                    {"body", {
                                     {"image", b64},
                                     {"width", w},
                                     {"height", h}
                    }}
            };

            callback->Success(val.dump());
            return true;
        } else if(type == "camera_capture") {
            sCameraStream->begin_capture();
            json val{
                    {"type", type},
                    {"body", {{"success", true}}}
            };

            callback->Success(val.dump());
            return true;
        }

        return false;
    }
}