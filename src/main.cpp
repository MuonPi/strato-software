
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

#include "strato-config.h"
#include "strato-lorawan.h"
#include "strato-sensors.h"
#include "CayenneLPP.h"
#include "logfile.h"




bool lorawan_inited = false;

Sensors StratoSensors;
Lorawan StratoLorawan;
CayenneLPP StratoPayload(255);


int main()
{
    auto start_time = std::chrono::steady_clock::now();
    auto last_message = std::chrono::steady_clock::now();
    const auto interval = std::chrono::seconds(LORAWAN_INTERVAL);
    const auto lorawan_timeout = std::chrono::seconds(LORAWAN_TIMEOUT);
    const auto sensorthread_timeout = std::chrono::seconds(SENSORTHREAD_TIMEOUT);
    
    StratoSensors.start();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    lorawan_inited = StratoLorawan.init();

    while(true)
    {

        if (StratoSensors.running == false || std::chrono::steady_clock::now() - StratoSensors.heartbeat.load() > sensorthread_timeout)
        {
            StratoSensors.stop();
            StratoSensors.start();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }


        StratoPayload.reset();
        StratoPayload.addGPS(0, StratoSensors.coordinates[0], StratoSensors.coordinates[1], StratoSensors.coordinates[2]);
        StratoPayload.addAnalogInput(1, StratoSensors.voltage_mean);
        StratoSensors.voltage_mean = 0;
        StratoSensors.voltage_count = 0;
        StratoPayload.addAnalogInput(2, StratoSensors.magnet_mean);
        StratoSensors.magnet_mean = 0;
        StratoSensors.magnet_count = 0;
        StratoPayload.addAnalogInput(3, StratoSensors.temperature_mean);
        StratoSensors.temperature_mean = 0;
        StratoSensors.temperature_count = 0;

        // std::cout << StratoPayload.getSize() << std::endl;
        // uint8_t* buffer = StratoPayload.getBuffer();
        // for(uint8_t i = 0; i < StratoPayload.getSize(); i++)
        // {
        //     buffer[i] = StratoPayload.getBuffer()[i];
        //     std::cout << static_cast<int>(buffer[i]) << " ";
        // }
        // std::cout << std::endl;

        if (lorawan_inited)
        {
            StratoLorawan.sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize());
            last_message = std::chrono::steady_clock::now();

            while(true)
            {
                StratoLorawan.runloop();
                if (txcomplete == true)
                    break;
                if (std::chrono::steady_clock::now() - last_message > lorawan_timeout)
                {
                    std::cerr << "LORAWAN timeout" << std::endl;
                    lorawan_inited = StratoLorawan.reset();
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            txcomplete = false;
        }
        else
        {
            lorawan_inited = StratoLorawan.reset();
        }


        
        std::this_thread::sleep_for(interval - ((std::chrono::steady_clock::now() - start_time) % interval));
    }


    StratoSensors.stop();
    return 0;
}

