#include <obd/response/BitArrayCodec.hpp>
#include "obd/ObdCommandList.hpp"

#include <mutex>
#include <obd/response/EngineLoadCodec.hpp>
#include <obd/response/Base40TemperatureCodec.hpp>
#include <obd/response/FuelTrimCodec.hpp>
#include <obd/response/FuelPressureCodec.hpp>
#include <obd/response/IntakeMainfoldPressureCodec.hpp>
#include <obd/response/EngineRpmCodec.hpp>
#include <obd/response/SpeedCodec.hpp>

namespace carpi::obd {
    std::map<std::string, ObdCommand> COMMAND_LIST{};

    static std::once_flag _command_list_call_flag{};

    void initialize_commands() {
        using namespace response;
        std::call_once(_command_list_call_flag, []() {
            COMMAND_LIST = {
                    {"PID01",              {1, "00", EcuType::ENGINE, std::make_shared<BitArrayCodec>(32)}},
                    {"DTC STATUS",         {1, "01", EcuType::ENGINE, std::make_shared<BitArrayCodec>(32)}},
                    {"FUEL STATUS",        {1, "03", EcuType::ENGINE, std::make_shared<BitArrayCodec>(16)}},
                    {"ENGINE LOAD",        {1, "04", EcuType::ENGINE, std::make_shared<EngineLoadCodec>()}},
                    {"ENGINE TEMPERATURE", {1, "05", EcuType::ENGINE, std::make_shared<Base40TemperatureCodec>()}},
                    {"SHORT FUEL TRIM 1",  {1, "06", EcuType::ENGINE, std::make_shared<FuelTrimCodec>()}},
                    {"LONG FUEL TRIM 1",   {1, "07", EcuType::ENGINE, std::make_shared<FuelTrimCodec>()}},
                    {"SHORT FUEL TRIM 2",  {1, "08", EcuType::ENGINE, std::make_shared<FuelTrimCodec>()}},
                    {"LONG FUEL TRIM 2",   {1, "09", EcuType::ENGINE, std::make_shared<FuelTrimCodec>()}},
                    {"FUEL PRESSURE",      {1, "0A", EcuType::ENGINE, std::make_shared<FuelPressureCodec>()}},
                    {"INTAKE PRESSURE",    {1, "0B", EcuType::ENGINE, std::make_shared<IntakeManifoldPressureCodec>()}},
                    {"RPM",                {1, "0C", EcuType::ENGINE, std::make_shared<EngineRpmCodec>()}},
                    {"SPEED",              {1, "0D", EcuType::ENGINE, std::make_shared<SpeedCodec>()}}
            };
        });
    }
}