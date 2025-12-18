
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

#include "strato-config.h"
#include "strato-lorawan.h"
#include "strato-sensors.h"
#include "CayenneLPP.h"

#include "ads1115.h"
#include "qmc5883.h"






Sensors StratoSensors;
Lorawan StratoLorawan;
CayenneLPP StratoPayload(255);


int main()
{
    auto start_time = std::chrono::steady_clock::now();
    const auto interval = std::chrono::milliseconds(LORAWAN_INTERVAL);
    
        StratoSensors.start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        StratoLorawan.init();


    while(true)
    {
        StratoPayload.reset();
        StratoPayload.addAnalogInput(0, StratoSensors.voltage_mean);
        StratoSensors.voltage_count = 0;
        StratoPayload.addAnalogInput(1, StratoSensors.temperature_mean);
        StratoSensors.temperature_count = 0;

        std::cout << StratoPayload.getSize() << std::endl;
        uint8_t* buffer = StratoPayload.getBuffer();
        for(uint8_t i = 0; i < StratoPayload.getSize(); i++)
        {
            // buffer[i] = static_cast<int>(StratoPayload.getBuffer()[i]);
            std::cout << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;


        StratoLorawan.sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize());

        while(!StratoLorawan.runloop())
            std::this_thread::sleep_for(std::chrono::seconds(1));

        
        std::this_thread::sleep_for(interval - ((std::chrono::steady_clock::now() - start_time) % interval));
    }


    StratoSensors.stop();
    return 0;
}


