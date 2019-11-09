cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_master)

add_executable(carpi_master src/main.cpp src/comm/CommServer.hpp src/comm/CommServer.cpp

    #    src/gl/GraphicsDevice.hpp src/gl/GraphicsDevice.cpp src/gl/Buffers.hpp src/gl/Buffers.cpp src/gl/VertexElement.hpp src/gl/VertexElement.cpp src/gl/Program.hpp
        src/ui/WebCore.hpp src/ui/WebCore.cpp src/ui/WebApplication.hpp src/ui/WebApplication.cpp src/main.hpp src/ui/WebClient.hpp src/ui/WebClient.cpp src/io/LocalSchemeHandler.hpp src/io/LocalSchemeHandler.cpp src/comm/IpcServer.hpp src/comm/IpcServer.cpp src/comm/IpcClientManager.hpp src/comm/IpcClientManager.cpp src/comm/IpcConnection.hpp src/comm/IpcConnection.cpp src/io/camera/camera_stream.hpp src/io/camera/camera_stream.cpp)

add_dependencies(carpi_master bluetooth_utils video_stream obd carpi_browser_subprocess ipc_common wiring_utils data_server)

find_package(Threads)

find_package(OpenGLES2 REQUIRED)
find_package(FFmpeg REQUIRED)

pkg_search_module(LIBYUV REQUIRED libyuv)

target_include_directories(carpi_master PUBLIC
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        ${CMAKE_SOURCE_DIR}/video_stream/include
        ${CMAKE_SOURCE_DIR}/obd/include
        ${CMAKE_SOURCE_DIR}/data_server/include
        ${OPENGLES2_INCLUDE_DIR}
        ${EGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${FFMPEG_INCLUDE_DIRS}
        ${LIBYUV_INCLUDE_DIRS}
        /usr/local/include/cef
)

target_link_libraries(carpi_master
        bluetooth_utils
        video_stream
        obd
        ipc_common
        wiring_utils
        data_server
        Threads::Threads
        ${OPENGLES2_LIBRARIES}
        ${EGL_LIBRARIES}
        ${FFMPEG_LIBRARIES}
        ${LIBYUV_LIBRARY_DIRS}
        ${LIBYUV_LIBRARIES}
        jpeg
        swscale
        avfilter
        stdc++fs
        /usr/local/lib/libcef_dll_wrapper.a
        /usr/local/bin/libcef.so
        X11)