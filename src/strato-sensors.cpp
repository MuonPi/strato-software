#include <iostream>
#include <cmath>
#include <chrono>

#include "strato-config.h"
#include "muonpi.h"
#include "ads1115.h"
#include "qmc5883.h"
#include "veml6075.h"
#include "bme280.h"
#include "ozone3click.h"
#include "strato-sensors.h"
#include "logfile.h"



Sensors::Sensors()
    : running(false)
{ }


Sensors::~Sensors()
{
    stop();
}


bool Sensors::start()
{
    std::cout << "SensorThread starting ..." << std::endl;
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true))
        return false;
    sensorThread = std::thread(&Sensors::threadFunc, this);
    return true;
}


bool Sensors::stop()
{
    std::cout << "SensorThread stopping ..." << std::endl;
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false))
        return false;
    if (sensorThread.joinable())
        sensorThread.join();
    return true;
}


void Sensors::threadFunc()
{
    try
    {
        std::cout << "SensorThread started" << std::endl;

        heartbeat = std::chrono::steady_clock::now();

        auto start_time = std::chrono::steady_clock::now();
        const auto sensors_interval = std::chrono::seconds(SENSORS_INTERVAL);
        const auto logfile_interval = std::chrono::minutes(LOGFILE_INTERVAL);
        
        std::string timestamp_value {0};
        std::string timestamp_filename {0};



        #ifdef MUONPI_USED
        MUONPI strato_muonpi;
        bool muonpi_inited = false;
        double XOR_temp = 0;
        double AND_temp = 0;
        double coordinates_temp[3] {0};
        #endif

        #ifdef ADS1115_ADDR
        ADS1115 strato_ads1115(ADS1115_ADDR);
        bool ads1115_inited = false;
        double voltage_temp = 0;
        #endif

        #ifdef QMC5883_ADDR
        QMC5883 strato_qmc5883(QMC5883_ADDR);
        bool qmc5883_inited = false;
        double magnetXYZ_temp[3] {0};
        // double temperature_temp = 0;
        #endif

        #ifdef VEML6075_ADDR
        VEML6075 strato_veml6075(VEML6075_ADDR);
        bool veml6075_inited = false;
        double uv_temp[4] {0};
        #endif

        #ifdef BME280_ADDR
        BME280 strato_bme280(BME280_ADDR);
        bool bme280_inited = true;
        TPH temperature_pressure_humidity_temp {0};
        #endif

        #ifdef OZONE3CLICK_LMP_ADDR
        #ifdef OZONE3CLICK_ADC_ADDR
        OZONE3CLICK strato_ozone3click(OZONE3CLICK_LMP_ADDR, OZONE3CLICK_ADC_ADDR);
        bool ozone3click_inited = false;
        double ozone_temp {0};
        #endif
        #endif


        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (running)
        {
            heartbeat = std::chrono::steady_clock::now();

            getTimestampFilename(timestamp_filename, logfile_interval);
            getTimestampValue(timestamp_value);

            

            #ifdef MUONPI_USED
            if (muonpi_inited)
            {
                if(strato_muonpi.getLogfilePath())
                {
                    if (strato_muonpi.getXOR(XOR_temp))
                    {
                        // std::cout << XOR_temp << std::endl;
                        XOR = XOR_temp;
                        XOR_mean = ((XOR_mean * XOR_count) + XOR_temp) / (XOR_count + 1);
                        XOR_count++;
                    }

                    if (strato_muonpi.getAND(AND_temp))
                    {
                        // std::cout << AND_temp << std::endl;
                        AND = AND_temp;
                        AND_mean = ((AND_mean * AND_count) + AND_temp) / (AND_count + 1);
                        AND_count++;
                    }

                    if (strato_muonpi.getCoordinates(coordinates_temp))
                    {
                        // std::cout << coordinates_temp[0] << " " << coordinates_temp[1] <<  " " << coordinates_temp[2] << std::endl;
                        for(size_t i; i < 3; i++)
                            coordinates[i] = coordinates_temp[i];
                    }
                }
                else
                    muonpi_inited = strato_muonpi.init();
            }
            else
                muonpi_inited = strato_muonpi.init();
            #endif



            #ifdef ADS1115_ADDR
            if (ads1115_inited)
            {
                if (strato_ads1115.getVoltage(voltage_temp))
                {
                    voltage_temp = voltage_temp * (VOLTAGE_DIVIDER_A0_R1 + VOLTAGE_DIVIDER_A0_R2) / VOLTAGE_DIVIDER_A0_R2;
                    voltage = voltage_temp;
                    voltage_mean = ((voltage_mean * voltage_count) + voltage_temp) / (voltage_count + 1);
                    voltage_count++;
                    writeLogfile("voltage", timestamp_filename, timestamp_value, &voltage_temp, 1);
                }
                else
                    ads1115_inited = strato_ads1115.init();
            }
            else
                ads1115_inited = strato_ads1115.init();
            #endif



            #ifdef QMC5883_ADDR
            if (qmc5883_inited)
            {
                if (strato_qmc5883.getMagneticFieldXYZ(magnetXYZ_temp))
                {
                    for(size_t i; i < 3; i++)
                        magnetXYZ[i] = magnetXYZ_temp[i];
                    magnet = std::sqrt(magnetXYZ_temp[0] * magnetXYZ_temp[0] + magnetXYZ_temp[1] * magnetXYZ_temp[1] + magnetXYZ_temp[2] * magnetXYZ_temp[2]);
                    magnet_mean = ((magnet_mean * magnet_count) + magnet) / (magnet_count + 1);
                    magnet_count++;
                    writeLogfile("magnetic_field", timestamp_filename, timestamp_value, magnetXYZ_temp, 3);
                }
                else
                    qmc5883_inited = strato_qmc5883.init();
            
                // if (strato_qmc5883.getTemperature(temperature_temp))
                // {
                //     temperature = temperature_temp;
                //     temperature_mean = ((temperature_mean * temperature_count) + temperature_temp) / (temperature_count + 1);
                //     temperature_count++;
                //     writeLogfile("temperature_inside", timestamp_filename, timestamp_value, &temperature_temp, 1);
                // }
                // else
                //     qmc5883_inited = strato_qmc5883.init();
            }
            else
                qmc5883_inited = strato_qmc5883.init();
            #endif



            #ifdef VEML6075_ADDR
            if (veml6075_inited)
            {
                if (strato_veml6075.getUV(uv_temp))
                {
                    for(size_t i; i < 4; i++)
                        uv[i] = uv_temp[i];
                    uv_mean = (uv_mean * uv_count) + ((uv_temp[0] + uv_temp[1]) / 2) / (uv_count + 1);
                    uv_count++;
                    writeLogfile("uv_index", timestamp_filename, timestamp_value, uv_temp, 4);
                    // std::cout << uv_temp[0] << "  " << uv_temp[1] << std::endl;
                }
                else
                    veml6075_inited = strato_veml6075.init();
            }
            else
                veml6075_inited = strato_veml6075.init();
            #endif



            #ifdef BME280_ADDR
            temperature_pressure_humidity_temp = {0};
            // // strato_bme280.softReset();
            if (bme280_inited)
            {
                temperature_pressure_humidity_temp = strato_bme280.getTPHValues();

                if (temperature_pressure_humidity_temp.T > -999.0)
                {
                    temperature = temperature_pressure_humidity_temp.T;
                    temperature_mean = ((temperature_mean * temperature_count) + temperature_pressure_humidity_temp.T) / (temperature_count + 1);
                    temperature_count++;
                    writeLogfile("temperature", timestamp_filename, timestamp_value, &temperature_pressure_humidity_temp.T, 1);
                }
                else
                    bme280_inited = strato_bme280.init();

                if (temperature_pressure_humidity_temp.P > -999.0)
                {
                    pressure = temperature_pressure_humidity_temp.P;
                    pressure_mean = ((pressure_mean * pressure_count) + temperature_pressure_humidity_temp.P) / (pressure_count + 1);
                    pressure_count++;
                    writeLogfile("pressure", timestamp_filename, timestamp_value, &temperature_pressure_humidity_temp.P, 1);
                }
                else
                    bme280_inited = strato_bme280.init();

                if (temperature_pressure_humidity_temp.H > -999.0)
                {
                    humidity = temperature_pressure_humidity_temp.H;
                    humidity_mean = ((humidity_mean * humidity_count) + temperature_pressure_humidity_temp.H) / (humidity_count + 1);
                    humidity_count++;
                    writeLogfile("humidity", timestamp_filename, timestamp_value, &temperature_pressure_humidity_temp.H, 1);
                }
                else
                    bme280_inited = strato_bme280.init();
            }
            else
                bme280_inited = strato_bme280.init();
            #endif



            #ifdef OZONE3CLICK_LMP_ADDR
            #ifdef OZONE3CLICK_ADC_ADDR
            if (ozone3click_inited)
            {
                if (strato_ozone3click.getOzone(ozone_temp))
                {
                    ozone = ozone_temp;
                    ozone_mean = (ozone_mean * ozone_count) + ozone_temp / (ozone_count + 1);
                    ozone_count++;
                    writeLogfile("ozone", timestamp_filename, timestamp_value, &ozone_temp, 1);
                    // std::cout << ozone_temp << std::endl;
                }
                else
                    ozone3click_inited = strato_ozone3click.init();
            }
            else
                ozone3click_inited = strato_ozone3click.init();
            #endif
            #endif




            std::this_thread::sleep_for(sensors_interval - ((std::chrono::steady_clock::now() - start_time) % sensors_interval));
        }

        std::cout << "SensorThread stopped" << std::endl;
        running = false;
    }
    catch(...)
    {
        std::cout << "SensorThread failed" << std::endl;
        running = false;
    }
}




