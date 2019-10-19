cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(obd)

add_library(obd SHARED
        src/ObdInterface.cpp
        include/obd/ObdCommand.hpp
        src/ObdCommand.cpp
        include/obd/response/ResponseConverter.hpp
        src/response/ResponseConverter.cpp include/obd/protocols/AbstractLegacyProtocol.hpp include/obd/protocols/AbstractCANProtocol.hpp src/protocols/AbstractCANProtocol.cpp src/protocols/AbstractLegacyProtocol.cpp include/obd/protocols/AbstractOBDProtocol.hpp src/protocols/AbstractOBDProtocol.cpp include/obd/msg/ObdMessage.hpp include/obd/msg/ObdFrame.hpp src/msg/ObdMessage.cpp src/msg/ObdFrame.cpp include/obd/EcuType.hpp include/obd/protocols/LegacyProtocols.hpp include/obd/protocols/CANProtocols.hpp src/protocols/LegacyProtocols.cpp src/protocols/CANProtocols.cpp include/obd/response/BitArrayResponseCodec.hpp src/response/BitArrayResponseCodec.cpp)

add_dependencies(obd bluetooth_utils common_utils)

target_include_directories(obd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/bluetooth_utils/include)
target_link_libraries(obd common_utils bluetooth_utils)