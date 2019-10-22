cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_browser_subprocess)

add_executable(carpi_browser_subprocess src/main.cpp)

add_dependencies(carpi_browser_subprocess common_utils)

target_include_directories(carpi_browser_subprocess PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include /usr/local/include/cef)
target_link_libraries(carpi_browser_subprocess common_utils /usr/local/lib/libcef_dll_wrapper.a /usr/local/bin/libcef.so)