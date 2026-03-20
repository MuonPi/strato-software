#include <iostream>
#include <cmath>
#include <chrono>

#include "strato-config.h"
#include "globals.h"
#include "muonpi.h"
#include "ads1115.h"
#include "qmc5883.h"
#include "veml6075.h"
#include "bme280.h"
#include "ozone3click.h"
#include "strato-sensors.h"
#include "logfile.h"



Sensors::Sensors(Globals& globals)
    : running(false), StratoGlobals(globals)
{}


Sensors::~Sensors()
{
    stop();
}


bool Sensors::start()
{
    std::cout << "SensorThread starting ..." << std::endl;

    #ifdef MUONPI_USED
    strato_muonpi = std::make_unique<MUONPI>();
    strato_muonpi->start();
    #endif

    bool expected = false;
    if (!running.compare_exchange_strong(expected, true))
        return false;

    sensorThread = std::thread(&Sensors::threadFunc, this);
    return true;
}


bool Sensors::stop()
{
    std::cout << "SensorThread stopping ..." << std::endl;

    #ifdef MUONPI_USED
    strato_muonpi->stop();
    #endif

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

        const auto start_time = std::chrono::steady_clock::now();
        const auto sensors_interval = std::chrono::seconds(SENSORS_INTERVAL);
        const auto logfile_interval = std::chrono::minutes(LOGFILE_INTERVAL);
        
        std::string timestamp_value {0};
        std::string timestamp_filename {0};




        #ifdef ADS1115_ADDR
        ADS1115 strato_ads1115(ADS1115_ADDR);
        bool ads1115_inited = false;
        #endif

        #ifdef QMC5883_ADDR
        QMC5883 strato_qmc5883(QMC5883_ADDR);
        bool qmc5883_inited = false;
        #endif

        #ifdef VEML6075_ADDR
        VEML6075 strato_veml6075(VEML6075_ADDR);
        bool veml6075_inited = false;
        #endif

        #ifdef BME280_ADDR
        BME280 strato_bme280(BME280_ADDR);
        bool bme280_inited = false;
        #endif

        #ifdef OZONE3CLICK_LMP_ADDR
        #ifdef OZONE3CLICK_ADC_ADDR
        OZONE3CLICK strato_ozone3click(OZONE3CLICK_LMP_ADDR, OZONE3CLICK_ADC_ADDR);
        bool ozone3click_inited = false;
        #endif
        #endif


        
        // std::this_thread::sleep_for(std::chrono::seconds(1));

        while (running)
        {
            heartbeat = std::chrono::steady_clock::now();

            getTimestampFilename(timestamp_filename, logfile_interval);
            getTimestampValue(timestamp_value);


            #ifdef MUONPI_USED
            // Should automatically reset if connection drops
            if (strato_muonpi->isConnected())
            {
                double position_temp[3] {0};
                if (strato_muonpi->getPosition(position_temp))
                {
                    if (position_temp[0] > -999)
                    {
                        for(uint8_t i{0}; i < 3; i++)
                            StratoGlobals.position[i] = position_temp[i];
                        // std::cout << "getPosition: " << position_temp[0] << " " << position_temp[1] <<  " " << position_temp[2] << std::endl;
                    }
                }

                double XOR_temp = 0;
                if (strato_muonpi->getXOR(XOR_temp))
                {
                    if (XOR_temp > -1)
                    {
                        StratoGlobals.XOR = XOR_temp;
                        StratoGlobals.XOR_mean = ((StratoGlobals.XOR_mean * StratoGlobals.XOR_count) + XOR_temp) / (StratoGlobals.XOR_count + 1);
                        StratoGlobals.XOR_count++;
                        // std::cout << "getXOR: "  << XOR_temp << std::endl;
                    }
                }

                double AND_temp = 0;
                if (strato_muonpi->getAND(AND_temp))
                {
                    if (AND_temp > -1)
                    {
                        StratoGlobals.AND = AND_temp;
                        StratoGlobals.AND_mean = ((StratoGlobals.AND_mean * StratoGlobals.AND_count) + AND_temp) / (StratoGlobals.AND_count + 1);
                        StratoGlobals.AND_count++;
                        // std::cout << "getAND: " <<  AND_temp << std::endl;
                    }
                }
            }
            #endif



            #ifdef ADS1115_ADDR
            if (ads1115_inited)
            {
                double voltage_temp = 0;
                if (strato_ads1115.getVoltage(voltage_temp))
                {
                    voltage_temp = voltage_temp * (VOLTAGE_DIVIDER_A0_R1 + VOLTAGE_DIVIDER_A0_R2) / VOLTAGE_DIVIDER_A0_R2;
                    StratoGlobals.voltage = voltage_temp;
                    StratoGlobals.voltage_mean = ((StratoGlobals.voltage_mean * StratoGlobals.voltage_count) + voltage_temp) / (StratoGlobals.voltage_count + 1);
                    StratoGlobals.voltage_count++;
                    writeLogfile("voltage", timestamp_filename, timestamp_value, &voltage_temp, 1);
                    // std::cout << "getVoltage: " << voltage_temp << std::endl;
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
                double magnetXYZ_temp[3] {0};
                if (strato_qmc5883.getMagneticFieldXYZ(magnetXYZ_temp))
                {
                    for(uint8_t i{0}; i < 3; i++)
                        StratoGlobals.magnetXYZ[i] = magnetXYZ_temp[i];
                    StratoGlobals.magnet = std::sqrt(magnetXYZ_temp[0] * magnetXYZ_temp[0] + magnetXYZ_temp[1] * magnetXYZ_temp[1] + magnetXYZ_temp[2] * magnetXYZ_temp[2]);
                    StratoGlobals.magnet_mean = ((StratoGlobals.magnet_mean * StratoGlobals.magnet_count) + StratoGlobals.magnet) / (StratoGlobals.magnet_count + 1);
                    StratoGlobals.magnet_count++;

                    double temperature_temp = 0;
                    if (strato_qmc5883.getTemperature(temperature_temp))
                    {
                        double magnet_temperature_temp[4] {0};
                        for(uint8_t i{0}; i < 3; i++)
                            magnet_temperature_temp[i] = magnetXYZ_temp[i];
                        magnet_temperature_temp[3] = temperature_temp;
                        writeLogfile("magnetic_field", timestamp_filename, timestamp_value, magnet_temperature_temp, 4);
                        // std::cout << "getMagnet: " << magnet_temperature_temp[0] << " " << magnet_temperature_temp[1] <<  " " << magnet_temperature_temp[2] << " " << magnet_temperature_temp[3] << std::endl;
                    }
                    else
                    {
                        writeLogfile("magnetic_field", timestamp_filename, timestamp_value, magnetXYZ_temp, 3);
                    }
                }
                else
                    qmc5883_inited = strato_qmc5883.init();
            }
            else
                qmc5883_inited = strato_qmc5883.init();
            #endif



            #ifdef VEML6075_ADDR
            if (veml6075_inited)
            {
                double uv_temp[4] {0};
                if (strato_veml6075.getUV(uv_temp))
                {
                    for(uint8_t i{0}; i < 4; i++)
                        StratoGlobals.uv[i] = uv_temp[i];
                    StratoGlobals.uv_mean = (StratoGlobals.uv_mean * StratoGlobals.uv_count) + ((uv_temp[0] + uv_temp[1]) / 2) / (StratoGlobals.uv_count + 1);
                    StratoGlobals.uv_count++;
                    writeLogfile("uv_index", timestamp_filename, timestamp_value, uv_temp, 4);
                    // std::cout << "getUV: " << uv_temp[0] << " " << uv_temp[1] << " " <<  uv_temp[2] << " " << uv_temp[3] << std::endl;
                }
                else
                    veml6075_inited = strato_veml6075.init();
            }
            else
                veml6075_inited = strato_veml6075.init();
            #endif



            #ifdef BME280_ADDR
            if (bme280_inited)
            {
                TPH tph_temp {0};
                tph_temp = strato_bme280.getTPHValues();


                if (tph_temp.T > -999.0)
                {
                    StratoGlobals.temperature = tph_temp.T;
                    StratoGlobals.temperature_mean = ((StratoGlobals.temperature_mean * StratoGlobals.temperature_count) + tph_temp.T) / (StratoGlobals.temperature_count + 1);
                    StratoGlobals.temperature_count++;
                    writeLogfile("temperature", timestamp_filename, timestamp_value, &tph_temp.T, 1);
                    // std::cout << "getTemperature: " << tph_temp.T << std::endl;
                }
                else
                    bme280_inited = strato_bme280.init();

                if (tph_temp.P > -999.0)
                {
                    StratoGlobals.pressure = tph_temp.P;
                    StratoGlobals.pressure_mean = ((StratoGlobals.pressure_mean * StratoGlobals.pressure_count) + tph_temp.P) / (StratoGlobals.pressure_count + 1);
                    StratoGlobals.pressure_count++;
                    writeLogfile("pressure", timestamp_filename, timestamp_value, &tph_temp.P, 1);
                    // std::cout << "getPressure: " << tph_temp.P << std::endl;
                }
                else
                    bme280_inited = strato_bme280.init();

                if (tph_temp.H > -999.0)
                {
                    StratoGlobals.humidity = tph_temp.H;
                    StratoGlobals.humidity_mean = ((StratoGlobals.humidity_mean * StratoGlobals.humidity_count) + tph_temp.H) / (StratoGlobals.humidity_count + 1);
                    StratoGlobals.humidity_count++;
                    writeLogfile("humidity", timestamp_filename, timestamp_value, &tph_temp.H, 1);
                    // std::cout << "getHumidity: " << tph_temp.H << std::endl;
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
                double ozone_temp = 0;
                if (strato_ozone3click.getOzone(ozone_temp))
                {
                    StratoGlobals.ozone = ozone_temp;
                    StratoGlobals.ozone_mean = (StratoGlobals.ozone_mean * StratoGlobals.ozone_count) + ozone_temp / (StratoGlobals.ozone_count + 1);
                    StratoGlobals.ozone_count++;
                    writeLogfile("ozone", timestamp_filename, timestamp_value, &ozone_temp, 1);
                    // std::cout << "getOzone: " << ozone_temp << std::endl;
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




