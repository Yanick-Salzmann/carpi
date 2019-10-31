cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(wiring_utils)

add_library(wiring_utils SHARED src/temperature.cpp include/wiring_utils/temperature.hpp src/wiring_context.cpp include/wiring_utils/wiring_context.hpp)

target_include_directories(wiring_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(wiring_utils /usr/lib/libwiringPi.so)