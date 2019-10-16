cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(obd)

add_library(obd SHARED src/ObdInterface.cpp include/obd/ObdCommand.hpp src/ObdCommand.cpp include/obd/ResponseConverter.hpp src/ResponseConverter.cpp include/obd/protocols/AbstractLegacyProtocol.hpp include/obd/protocols/AbstractCANProtocol.hpp src/protocols/AbstractCANProtocol.cpp src/protocols/AbstractLegacyProtocol.cpp)

add_dependencies(obd bluetooth_utils common_utils)

target_include_directories(obd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/bluetooth_utils/include)
target_link_libraries(obd common_utils bluetooth_utils)