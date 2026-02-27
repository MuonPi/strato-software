
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
    std::this_thread::sleep_for(std::chrono::seconds(5));




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
        // std::cout << static_cast<float>(StratoSensors.coordinates[0]) << " " << static_cast<float>(StratoSensors.coordinates[1]) <<  " " << static_cast<float>(StratoSensors.coordinates[2]) << std::endl;
        StratoPayload.addGPS(2, static_cast<float>(StratoSensors.coordinates[0]), static_cast<float>(StratoSensors.coordinates[1]), static_cast<float>(StratoSensors.coordinates[2]));
        StratoPayload.addAnalogInput(3, static_cast<float>(StratoSensors.voltage_mean));
        StratoSensors.voltage_mean = 0;
        StratoSensors.voltage_count = 0;
        StratoPayload.addAnalogInput(4, StratoSensors.XOR_mean);
        StratoSensors.XOR_mean = 0;
        StratoSensors.XOR_count = 0;
        StratoPayload.addAnalogInput(5, StratoSensors.AND_mean);
        StratoSensors.AND_mean = 0;
        StratoSensors.AND_count = 0;
        StratoPayload.addBarometricPressure(7, static_cast<float>(StratoSensors.pressure_mean / 100));
        StratoSensors.pressure_mean = 0;
        StratoSensors.pressure_count = 0;
        StratoPayload.addTemperature(8, static_cast<float>(StratoSensors.temperature_mean));
        StratoSensors.temperature_mean = 0;
        StratoSensors.temperature_count = 0;

        // std::cout << "Ozone: " << StratoSensors.ozone << std::endl;

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
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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








// // #include <fcntl.h>
// // #include <unistd.h>
// // #include <sys/ioctl.h>
// // #include <linux/i2c-dev.h>
// // #include <cstdint>
// // #include <chrono>
// // #include <thread>
// // #include <iostream>


// // bool initVEML6075(int fd)
// // {
// //     constexpr uint8_t REG_CONF = 0x00;

// //     // UV_IT = 800ms, continuous, SD=0
// //     uint16_t config = (0b100 << 4);

// //     uint8_t cfg[3] = {
// //         REG_CONF,
// //         uint8_t(config & 0xFF),
// //         uint8_t(config >> 8)
// //     };

// //     if (write(fd, cfg, 3) != 3)
// //         return false;

// //     std::this_thread::sleep_for(std::chrono::milliseconds(900));
// //     return true;
// // }

// // bool readVEML6075Raw(
// //     int fd,
// //     uint16_t& uva,
// //     uint16_t& uvb,
// //     uint16_t& comp1,
// //     uint16_t& comp2
// // )
// // {
// //     auto read16 = [&](uint8_t reg, uint16_t& out) -> bool {
// //         if (write(fd, &reg, 1) != 1)
// //             return false;
// //         uint8_t buf[2];
// //         if (read(fd, buf, 2) != 2)
// //             return false;
// //         out = (uint16_t(buf[1]) << 8) | buf[0];
// //         return true;
// //     };

// //     return read16(0x07, uva) &&
// //            read16(0x09, uvb) &&
// //            read16(0x0A, comp1) &&
// //            read16(0x0B, comp2);
// // }





// // uint16_t uva, uvb, comp1, comp2;

// // int main()
// // {
// //     int fd = open("/dev/i2c-1", O_RDWR);
// //     ioctl(fd, I2C_SLAVE, 0x10);

// //     initVEML6075(fd);

// //     while (true) {
// //         readVEML6075Raw(fd, uva, uvb, comp1, comp2);

// //         std::cout
// //             << "UVA=" << uva
// //             << " UVB=" << uvb
// //             << " C1=" << comp1
// //             << " C2=" << comp2
// //             << std::endl;

// //         std::this_thread::sleep_for(std::chrono::seconds(1));
// //     }
// // }

