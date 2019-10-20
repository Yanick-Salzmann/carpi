cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(obd)

add_library(obd SHARED
        src/ObdInterface.cpp
        include/obd/ObdCommand.hpp
        src/ObdCommand.cpp
        include/obd/response/ResponseConverter.hpp
        src/response/ResponseConverter.cpp include/obd/protocols/AbstractLegacyProtocol.hpp include/obd/protocols/AbstractCANProtocol.hpp src/protocols/AbstractCANProtocol.cpp src/protocols/AbstractLegacyProtocol.cpp include/obd/protocols/AbstractOBDProtocol.hpp src/protocols/AbstractOBDProtocol.cpp include/obd/msg/ObdMessage.hpp include/obd/msg/ObdFrame.hpp src/msg/ObdMessage.cpp src/msg/ObdFrame.cpp include/obd/EcuType.hpp include/obd/protocols/LegacyProtocols.hpp include/obd/protocols/CANProtocols.hpp src/protocols/LegacyProtocols.cpp src/protocols/CANProtocols.cpp include/obd/response/BitArrayCodec.hpp src/response/BitArrayCodec.cpp include/obd/response/EngineLoadCodec.hpp src/response/EngineLoadCodec.cpp include/obd/response/Base40TemperatureCodec.hpp src/response/Base40TemperatureCodec.cpp include/obd/response/FuelTrimCodec.hpp src/response/FuelTrimCodec.cpp include/obd/response/FuelPressureCodec.hpp src/response/FuelPressureCodec.cpp include/obd/response/EngineRpmCodec.hpp src/response/EngineRpmCodec.cpp include/obd/response/IntakeMainfoldPressureCodec.hpp src/response/IntakeManifoldPressureCodec.cpp include/obd/response/SpeedCodec.hpp src/response/SpeedCodec.cpp include/obd/ObdCommandList.hpp src/ObdCommandList.cpp include/obd/response/TimingAdvanceCodec.hpp src/response/TimingAdvanceCodec.cpp include/obd/response/AirflowRateCodec.hpp src/response/AirflowRateCodec.cpp include/obd/response/ThrottlePositionCodec.hpp src/response/ThrottlePositionCodec.cpp include/obd/response/OxygenSensorCodec.hpp src/response/OxygenSensorCodec.cpp include/obd/response/EngineRuntimeCodec.hpp src/response/EngineRuntimeCodec.cpp)

add_dependencies(obd bluetooth_utils common_utils)

target_include_directories(obd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/bluetooth_utils/include)
target_link_libraries(obd common_utils bluetooth_utils)