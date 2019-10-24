#ifndef CARPI_COMMON_UTILS_SINGLETON_HPP
#define CARPI_COMMON_UTILS_SINGLETON_HPP

#include <memory>

namespace carpi::utils {
    template<typename T>
    class Singleton {
    public:
        static std::shared_ptr<T> instance() {
            static auto instance = std::make_shared<T>();
            return instance;
        }
    };
}

#endif //CARPI_COMMON_UTILS_SINGLETON_HPP
