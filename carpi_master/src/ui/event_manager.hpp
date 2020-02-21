#ifndef CARPI_CARPI_EVENT_MANAGER_HPP
#define CARPI_CARPI_EVENT_MANAGER_HPP

#include <common_utils/singleton.hpp>
#include <functional>
#include <nlohmann/json.hpp>
#include <map>
#include <common_utils/log.hpp>

namespace carpi::ui {
    class NoOp {
    public:
        static NoOp from_json(const nlohmann::json&) {
            return {};
        }
    };

    class EventManager : public utils::singleton<EventManager> {
        class EventHandler {
        public:
            virtual ~EventHandler() = default;

            virtual nlohmann::json execute(const std::string& payload) = 0;
        };

        template<typename R, typename T>
        class SpecificEventHandler : public EventHandler {
            std::function<R (const T&)> _callback;

        public:
            explicit SpecificEventHandler(std::function<R (const T&)> callback) : _callback{std::move(callback)} {

            }

            nlohmann::json execute(const std::string &payload) override {
                const auto json_req = nlohmann::json::parse(payload);
                const auto response = _callback(T::from_json(json_req));
                return response.to_json();
            }
        };

        LOGGER;

        std::function<void(const std::string&)> _event_callback{};
        std::multimap<std::string, std::shared_ptr<EventHandler>> _event_handlers{};

    public:
        void apply_event_callback(std::function<void(const std::string&)> callback) {
            _event_callback = std::move(callback);
        }

        void dispatch_event(const std::string& event_name, const std::string& payload);

        template<typename T>
        void dispatch_event(const std::string& event_name, const T& value) {
            dispatch_event(event_name, value.to_json());
        }

        template<typename T, typename R>
        void register_event_handler(const std::string& event_name, std::function<R (const T&)> callback) {
            _event_handlers.emplace(event_name, std::make_shared<SpecificEventHandler<R, T>>(callback));
        }

        nlohmann::json handle_event(const std::string& event_name, const std::string& payload) {
            const auto range = _event_handlers.equal_range(event_name);
            if(range.first == range.second) {
                log->warn("Ignoring event {} because there are no registered handlers", event_name);
                return {};
            }

            nlohmann::json response{};
            for(auto itr = range.first; itr != range.second; ++itr) {
                const auto resp = itr->second->execute(payload);
                if(!resp.empty()) {
                    response = resp;
                }
            }

            return response;
        }
    };
}

#define sUiEventMgr (carpi::ui::EventManager::instance())

#endif //CARPI_CARPI_EVENT_MANAGER_HPP
