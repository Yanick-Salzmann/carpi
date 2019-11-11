#include "event_manager.hpp"
#include <nlohmann/json.hpp>

namespace carpi::ui {
    LOGGER_IMPL(EventManager);

    using nlohmann::json;

    void EventManager::dispatch_event(const std::string &event_name, const std::string &payload) {
        _event_callback(json{
                {"type", event_name},
                {"payload", payload}
        }.dump());
    }
}