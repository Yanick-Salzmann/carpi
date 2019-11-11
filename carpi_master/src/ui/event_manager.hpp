#ifndef CARPI_CARPI_EVENT_MANAGER_HPP
#define CARPI_CARPI_EVENT_MANAGER_HPP

#include <common_utils/singleton.hpp>
#include <functional>

namespace carpi::ui {
    class EventManager : public utils::Singleton<EventManager> {
        std::function<void(const std::string&)> _event_callback;

    public:
        void apply_event_callback(std::function<void(const std::string&)> callback) {
            _event_callback = std::move(callback);
        }
    };
}

#define sUiEventMgr (carpi::ui::EventManager::instance())

#endif //CARPI_CARPI_EVENT_MANAGER_HPP
