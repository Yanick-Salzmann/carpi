cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_browser_subprocess)

configure_file(resources/config.toml resources/config.toml)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data/CH_addresses.csv
        COMMAND unzip -n ${CMAKE_CURRENT_SOURCE_DIR}/data/CH_addresses.zip -d ${CMAKE_CURRENT_SOURCE_DIR}/data
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/CH_addresses.zip
)

add_executable(carpi_browser_subprocess data/CH_addresses.csv src/main.cpp src/camera_frame_callback.hpp src/camera_frame_callback.cpp src/main_app.hpp src/main_app.cpp src/env_var_callback.hpp src/env_var_callback.cpp src/gps_listener_thread.hpp src/gps_listener_thread.cpp src/gps_callback.hpp src/gps_callback.cpp src/ch_address_reader.hpp src/ch_address_reader.cpp)

add_dependencies(carpi_browser_subprocess common_utils ipc_common net_utils gps_utils)

find_package(Threads)

target_include_directories(carpi_browser_subprocess PUBLIC
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${CMAKE_SOURCE_DIR}/ipc_common/include
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_SOURCE_DIR}/gps/include
        /usr/local/include/cef)

target_link_libraries(carpi_browser_subprocess
        common_utils
        ipc_common
        /usr/local/lib/libcef_dll_wrapper.a
        /usr/local/bin/libcef.so
        stdc++fs
        Threads::Threads
        gps_utils
        net_utils
)