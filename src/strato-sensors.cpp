#include <iostream>

#include "strato-config.h"
#include "ads1115.h"
#include "qmc5883.h"
#include "strato-sensors.h"




Sensors::Sensors()
    : running(false)
{ }


Sensors::~Sensors()
{
    stop();
}


bool Sensors::start()
{
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true))
        return false;

    sensorThread = std::thread(&Sensors::threadFunc, this);
    return true;
}


bool Sensors::stop()
{
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false))
        return false;

    if (sensorThread.joinable())
        sensorThread.join();
    return true;
}


void Sensors::threadFunc()
{
    std::cout << "SensorThread started" << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    const auto interval = std::chrono::milliseconds(SENSORS_INTERVAL);

    ADS1115 strato_ads1115(ADS1115_ADDR);
    QMC5883 strato_qmc5883(QMC5883_ADDR);
    
    strato_ads1115.init();
    strato_qmc5883.init();

    double voltage_temp = 0;
    double magnet_temp = 0;
    double temperature_temp = 0;

    while (running)
    {
        if (strato_ads1115.getVoltage(voltage_temp))
        {
            voltage_temp = voltage_temp * (VOLTAGE_DIVIDER_A0_R1 + VOLTAGE_DIVIDER_A0_R2) / VOLTAGE_DIVIDER_A0_R2;
            voltage = voltage_temp;
            voltage_mean = ((voltage_mean * voltage_count) + voltage_temp) / (voltage_count + 1);
            voltage_count++;
        }

        if (strato_qmc5883.getMagneticField(magnet_temp))
        {
            magnet = magnet_temp;
            magnet_mean = ((magnet_mean * magnet_count) + magnet_temp) / (magnet_count + 1);
            magnet_count++;
        }

        if (strato_qmc5883.getTemperature(temperature_temp))
        {
            temperature = temperature_temp;
            temperature_mean = ((temperature_mean * temperature_count) + temperature_temp) / (temperature_count + 1);
            temperature_count++;
        }

        std::this_thread::sleep_for(interval - ((std::chrono::steady_clock::now() - start_time) % interval));
    }

    std::cout << "SensorThread stopped" << std::endl;
}




