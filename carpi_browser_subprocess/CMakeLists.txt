cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_browser_subprocess)

add_executable(carpi_browser_subprocess src/main.cpp src/camera_frame_callback.hpp src/camera_frame_callback.cpp src/main_app.hpp src/main_app.cpp src/env_var_callback.hpp src/env_var_callback.cpp)

add_dependencies(carpi_browser_subprocess common_utils)

find_package(Threads)

target_include_directories(carpi_browser_subprocess PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include /usr/local/include/cef)
target_link_libraries(carpi_browser_subprocess common_utils /usr/local/lib/libcef_dll_wrapper.a /usr/local/bin/libcef.so Threads::Threads)