#include "common_utils/log.hpp"
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace carpi::utils {
    std::string demangle(const char* name) {
        int status = -1;

        std::unique_ptr<char, void(*)(void*)> res {
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free
        };

        return (status==0) ? res.get() : name ;
    }

    void Logger::init_logger (const std::string& name) {
        _logger = spdlog::stdout_color_mt (name);
        _logger->set_pattern ("[%Y-%m-%d %H:%I:%S.%e] [%P:%t] [%n] %^[%l]%$ %v");
#ifndef NDEBUG
        _logger->set_level (spdlog::level::debug);
#else
        _logger->set_level (spdlog::level::info);
#endif
    }

    std::string Logger::sanitize_type_name(const std::string &name) {
        auto actual_name = demangle(name.c_str());

        const auto idx_space = actual_name.find(' ');
        if(idx_space == std::string::npos) {
            return actual_name;
        }

        return actual_name.substr(idx_space + 1);
    }
}