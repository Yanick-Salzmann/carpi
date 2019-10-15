cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(common_utils)

add_library(common_utils SHARED src/log/Logger.cpp include/common_utils/error.hpp src/error.cpp include/common_utils/any.hpp src/any.cpp)

add_dependencies(common_utils spdlog)

target_include_directories(common_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/third_party/spdlog/include)