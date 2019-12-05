cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(wiring_utils)

add_library(wiring_utils SHARED src/temperature.cpp include/wiring_utils/temperature.hpp src/wiring_context.cpp include/wiring_utils/wiring_context.hpp include/wiring_utils/bme280.hpp src/bme280.cpp include/wiring_utils/gpio.hpp src/gpio.cpp include/wiring_utils/serial_interface.hpp src/serial_interface.cpp)

add_dependencies(wiring_utils common_utils)

target_include_directories(wiring_utils PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(wiring_utils /usr/lib/libwiringPi.so common_utils)