cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_master)

add_executable(carpi_master
        src/main.cpp src/comm/comm_server.hpp src/comm/comm_server.cpp
        #src/gl/Buffers.cpp src/gl/Buffers.hpp src/gl/graphics_device.hpp src/gl/graphics_device.cpp src/gl/Program.hpp src/gl/vertex_element.cpp src/gl/vertex_element.hpp
        src/ui/web_core.hpp src/ui/web_core.cpp src/ui/web_application.hpp src/ui/web_application.cpp src/main.hpp src/ui/web_client.hpp src/ui/web_client.cpp src/io/local_scheme_handler.hpp src/io/local_scheme_handler.cpp src/comm/ipc_server.hpp src/comm/ipc_server.cpp src/comm/ipc_client_manager.hpp src/comm/ipc_client_manager.cpp src/comm/ipc_connection.hpp src/comm/ipc_connection.cpp src/io/camera/camera_stream.hpp src/io/camera/camera_stream.cpp src/ui/event_manager.hpp src/ui/event_manager.cpp src/ui/events/bluetooth_events.hpp src/ui/events/bluetooth_events.cpp src/io/obd/ObdConnectionManager.hpp src/io/obd/ObdConnectionManager.cpp)


add_dependencies(carpi_master bluetooth_utils video_stream obd carpi_browser_subprocess ipc_common wiring_utils data_server json net_utils gps_utils)

find_package(Threads)

#find_package(OpenGLES2 REQUIRED)
find_package(FFmpeg REQUIRED)

target_include_directories(carpi_master PUBLIC
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        ${CMAKE_SOURCE_DIR}/video_stream/include
        ${CMAKE_SOURCE_DIR}/obd/include
        ${CMAKE_SOURCE_DIR}/data_server/include
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_SOURCE_DIR}/gps/include
        ${CMAKE_BINARY_DIR}/third_party/json/include
        #${OPENGLES2_INCLUDE_DIR}
        #${EGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${FFMPEG_INCLUDE_DIRS}
        /usr/local/include/cef
        )

target_link_libraries(carpi_master
        bluetooth_utils
        video_stream
        obd
        ipc_common
        wiring_utils
        data_server
        net_utils
        gps_utils
        Threads::Threads
        #${OPENGLES2_LIBRARIES}
        #${EGL_LIBRARIES}
        ${FFMPEG_LIBRARIES}
        jpeg
        swscale
        avfilter
        stdc++fs
        /usr/local/lib/libcef_dll_wrapper.a
        /usr/local/bin/libcef.so
        X11
        )