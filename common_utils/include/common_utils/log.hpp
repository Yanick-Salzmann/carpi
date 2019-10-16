#ifndef CARPI_COMMON_UTILS_LOGGER_HPP
#define CARPI_COMMON_UTILS_LOGGER_HPP

#include <string>
#include <memory>
#include <spdlog/spdlog.h>

namespace carpi::utils {
    class Logger {
        std::shared_ptr<spdlog::logger> _logger;

        void init_logger (const std::string& name);
        std::string static sanitize_type_name(const std::string& name);

    public:
        template<typename T>
        struct type_tag {

        };

        template<typename T>
        explicit Logger(type_tag<T>&& tag) noexcept {
            init_logger (sanitize_type_name(typeid(T).name()));
        }

        explicit Logger(const std::string& name) {
            init_logger (name);
        }

        const std::shared_ptr<spdlog::logger>& operator->() const {
            return _logger;
        }
    };
}

#define LOGGER static carpi::utils::Logger log
#define LOGGER_IMPL(T) carpi::utils::Logger T::log = carpi::utils::Logger{carpi::utils::Logger::type_tag<T>()}

#endif //CARPI_COMMON_UTILS_LOGGER_HPP
