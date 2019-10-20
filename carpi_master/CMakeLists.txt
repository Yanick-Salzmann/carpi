cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_master)

add_executable(carpi_master src/main.cpp src/comm/CommServer.hpp src/comm/CommServer.cpp

    #    src/gl/GraphicsDevice.hpp src/gl/GraphicsDevice.cpp src/gl/Buffers.hpp src/gl/Buffers.cpp src/gl/VertexElement.hpp src/gl/VertexElement.cpp src/gl/Program.hpp
        )

add_dependencies(carpi_master bluetooth_utils video_stream obd)

find_package(Threads)

find_package(OpenGLES2 REQUIRED)

target_include_directories(carpi_master PUBLIC
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        ${CMAKE_SOURCE_DIR}/video_stream/include
        ${CMAKE_SOURCE_DIR}/obd/include
        ${OPENGLES2_INCLUDE_DIR}
        ${EGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(carpi_master bluetooth_utils video_stream obd Threads::Threads ${OPENGLES2_LIBRARIES} ${EGL_LIBRARIES})