#include <iostream>
#include <video_stream/H264Conversion.hpp>
#include <bluetooth_utils/BluetoothManager.hpp>
#include <bluetooth_utils/BluetoothDevice.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>
#include "comm/CommServer.hpp"

#include <obd/ObdInterface.hpp>
#include <obd/ObdCommandList.hpp>
#include <ui/WebCore.hpp>
#include <video_stream/H264Stream.hpp>

#include "main.hpp"
#include <fstream>

#include <wiringPi.h>

class MemoryStreamSource : public carpi::video::IStreamSource {
    LOGGER;

    std::vector<uint8_t> _data;
    std::size_t _position = 0;

public:
    explicit MemoryStreamSource(std::vector<uint8_t> data) : _data(std::move(data)) {}

    size_t read(void *buffer, std::size_t num_bytes) override {
        if (_position >= _data.size()) {
            return 0;
        }

        const auto to_read = std::min(num_bytes, _data.size() - _position);
        memcpy(buffer, &_data[_position], to_read);
        _position += to_read;
        return to_read;
    }
};

LOGGER_IMPL(MemoryStreamSource);

namespace carpi {
    int _argc;
    char **_argv;

    void temperature_loop() {
#define MAXTIMINGS	85
#define DHTPIN		23
        int dht11_dat[5] = { 0, 0, 0, 0, 0 };

        uint8_t laststate	= HIGH;
        uint8_t counter		= 0;
        uint8_t j		= 0, i;
        float	f;

        dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

        pinMode( DHTPIN, OUTPUT );
        digitalWrite( DHTPIN, LOW );
        delay( 18 );
        digitalWrite( DHTPIN, HIGH );
        delayMicroseconds( 40 );
        pinMode( DHTPIN, INPUT );

        for ( i = 0; i < MAXTIMINGS; i++ )
        {
            counter = 0;
            while ( digitalRead( DHTPIN ) == laststate )
            {
                counter++;
                delayMicroseconds( 1 );
                if ( counter == 255 )
                {
                    break;
                }
            }
            laststate = digitalRead( DHTPIN );

            if ( counter == 255 )
                break;

            if ( (i >= 4) && (i % 2 == 0) )
            {
                dht11_dat[j / 8] <<= 1;
                if ( counter > 16 )
                    dht11_dat[j / 8] |= 1;
                j++;
            }
        }

        if ( (j >= 40) &&
             (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
        {
            printf( "\rHumidity = %02d.%02d %% Temperature = %02d.%02d C",
                    dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]);
        }else  {
            printf( "\rData not good, skip                                       " );
        }
    }

    int main(int argc, char *argv[]) {
        if ( wiringPiSetup() == -1 )
            exit( 1 );

        while(true) {
            temperature_loop();
            delay(1000);
        }

        _argc = argc;
        _argv = argv;

        video::H264Conversion::initialize_ffmpeg();
        utils::Logger log{"main"};
        ui::WebCore core{};

        CommServer server{};

        log->info("Press ENTER to shut down application");
        std::cin.sync();
        std::string line{};
        std::getline(std::cin, line);

        core.manual_shutdown();

        server.shutdown_acceptor();
        return 0;
    }
}

int main(int argc, char *argv[]) {
    return carpi::main(argc, argv);
}