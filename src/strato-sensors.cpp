#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include "strato-config.h"
#include "globals.h"
#include "muonpi-connector.h"
#include "i2c/ads1115.h"
#include "i2c/as7331.h"
#include "i2c/as7343.h"
#include "i2c/bme280.h"
#include "i2c/ltr390uv01.h"
#include "i2c/mpu6050.h"
#include "i2c/qmc5883.h"
#include "i2c/sht31.h"
#include "i2c/ozone3click.h"

#include "strato-sensors.h"
#include "logfile.h"




// ========================== Initialization ==========================


#ifdef MUONPI_USED
MUONPI& StratoMUONPI()
{
    static MUONPI device{};
    return device;
}
#endif


#ifdef ADS1115_ADDR
ADS1115& StratoADS1115()
{
    static ADS1115 device(ADS1115_ADDR);
    return device;
}
bool ads1115_inited = false;
bool init_ads1115()
{
    StratoADS1115().setAGC(false);
    StratoADS1115().setPga(ADS1115::CFG_PGA::PGA4V);
    StratoADS1115().setContinuousSampling(true);
    std::cout << "ADS1115 inited" << std::endl;
    return StratoADS1115().identify();
}
#endif


#ifdef BME280_ADDR
BME280& StratoBME280()
{
    static BME280 device(BME280_ADDR);
    return device;
}
bool bme280_inited = false;
bool init_bme280()
{
    return StratoBME280().init();
}
#endif


#ifdef QMC5883_ADDR
QMC5883& StratoQMC5883()
{
    static QMC5883 device(QMC5883_ADDR);
    return device;
}
bool qmc5883_inited = false;
bool init_qmc5883()
{
    return StratoQMC5883().init();
}
#endif


#ifdef MPU6050_ADDR
MPU6050& StratoMPU6050()
{
    static MPU6050 device(MPU6050_ADDR);
    return device;
}
constexpr uint8_t MPU6050_VECTOR_CHANNELS{3};
bool mpu6050_inited = false;
bool init_mpu6050()
{
    return StratoMPU6050().init(MPU6050::GYRO_RANGE::DPS_250, MPU6050::ACCEL_RANGE::G_2, MPU6050::DLPF::ACCEL_5HZ_GYRO_5HZ, 199);
}
#endif


#ifdef OZONE3CLICK_LMP_ADDR
#ifdef OZONE3CLICK_ADC_ADDR
OZONE3CLICK& StratoOZONE3CLICK()
{
    static OZONE3CLICK device(OZONE3CLICK_LMP_ADDR, OZONE3CLICK_ADC_ADDR);
    return device;
}
bool ozone3click_inited = false;
bool init_ozone3click()
{
    return StratoOZONE3CLICK().init();
}
#endif
#endif


#ifdef LTR390UV01_ADDR
LTR390UV01& StratoLTR390UV01()
{
    static LTR390UV01 device(LTR390UV01_ADDR);
    return device;
}
bool ltr390uv01_inited = false;
bool init_ltr390uv01()
{
    StratoLTR390UV01().init();
    return StratoLTR390UV01().devicePresent();
}
#endif


#ifdef AS7331_ADDR
AS7331& StratoAS7331()
{
    static AS7331 device(AS7331_ADDR);
    return device;
}
constexpr uint8_t AS7331_UV_CHANNELS{3};
bool as7331_inited = false;
bool init_as7331()
{
    StratoAS7331().reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    StratoAS7331().setGain(AS7331::GAIN::_2048x);
    return StratoAS7331().identify();
}
#endif



#ifdef AS7343_ADDR
AS7343& StratoAS7343()
{
    static AS7343 device(AS7343_ADDR);
    return device;
}
constexpr uint8_t AS7343_SPECTRUM_CHANNELS{18};
bool as7343_inited = false;
bool init_as7343()
{
    AS7343::Config config_as7343{};
    config_as7343.autoSmuxMode = AS7343::AUTO_SMUX_MODE::_18Ch;
    config_as7343.fifoMap = 0x7e;
    config_as7343.gain = AS7343::GAIN::_16x;
    config_as7343.atime = 0x09;
    config_as7343.astep = 3596;
    config_as7343.ledAct = false;
    config_as7343.ledDrive = 0b0100;
    if (StratoAS7343().identify())
    {
        StratoAS7343().reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        StratoAS7343().init(config_as7343);
        return true;
    }
    else
        return false;
}
#endif



#ifdef SHT31_ADDR
SHT31& StratoSHT31()
{
    static SHT31 device(SHT31_ADDR);
    return device;
}
bool sht31_inited = false;
bool init_sht31()
{
    return StratoSHT31().devicePresent();
}
#endif








Sensors::Sensors(Globals& globals)
    : StratoGlobals(globals)
{}



Sensors::~Sensors()
{}


// ========================== Readout ==========================


bool Sensors::execute()
{
    try
    {
        if(!activated)
            return false;
        
        active = true;
        starttime = std::chrono::steady_clock::now();

        // auto now = std::chrono::system_clock::now();
        // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        // std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        // std::tm* tm = std::localtime(&now_c);
        // std::cout << std::put_time(tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count() << std::endl;


        std::string timestamp_value {0};
        std::string timestamp_filename {0};
        const auto logfile_interval = std::chrono::minutes(LOGFILE_INTERVAL);

        getTimestampFilename(timestamp_filename, logfile_interval);
        getTimestampValue(timestamp_value);


        if(inited == false)
        {
            ads1115_inited = false;
            qmc5883_inited = false;
            ltr390uv01_inited = false;
            as7331_inited = false;
            as7343_inited = false;
            bme280_inited = false;
            sht31_inited = false;
            mpu6050_inited = false;
            ozone3click_inited = false;
            inited = true;
        }
        

        #ifdef MUONPI_USED
        // Should automatically reset if connection drops
        if (StratoMUONPI().isConnected())
        {
            uint8_t gps_fix_temp = 0;
            if (StratoMUONPI().getGpsFix(gps_fix_temp))
                StratoGlobals.gps_fix = gps_fix_temp;

            double position_temp[3] {0};
            if (StratoMUONPI().getPosition(position_temp))
            {
                if (position_temp[0] > -999)
                {
                    for(uint8_t i{0}; i < 3; i++)
                        StratoGlobals.position[i] = position_temp[i];
                    StratoGlobals.position_update_count++;
                    // std::cout << "getPosition: " << position_temp[0] << " " << position_temp[1] <<  " " << position_temp[2] << std::endl;
                }
            }

            double XOR_temp = 0;
            if (StratoMUONPI().getXOR(XOR_temp))
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
            if (StratoMUONPI().getAND(AND_temp))
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
            ADS1115::Sample battery_voltage_sample = (StratoADS1115()).getSample(ADS1115::CH0);
            if (battery_voltage_sample != ADS1115::InvalidSample)
            {
                double battery_voltage_temp = battery_voltage_sample.voltage;
                battery_voltage_temp = battery_voltage_temp * (VOLTAGE_DIVIDER_A0_R1 + VOLTAGE_DIVIDER_A0_R2) / VOLTAGE_DIVIDER_A0_R2;
                StratoGlobals.battery_voltage = battery_voltage_temp;
                StratoGlobals.battery_voltage_mean = ((StratoGlobals.battery_voltage_mean * StratoGlobals.battery_voltage_count) + battery_voltage_temp) / (StratoGlobals.battery_voltage_count + 1);
                StratoGlobals.battery_voltage_count++;
                writeLogfile("battery_voltage_ads1115_ch0", timestamp_filename, timestamp_value, &battery_voltage_temp, 1);
                // std::cout << "getVoltage: " << voltage_temp << std::endl;
            }
            else
                ads1115_inited = init_ads1115();
            
            ADS1115::Sample solar_voltage_sample = StratoADS1115().getSample(ADS1115::CH0);
            if (solar_voltage_sample != ADS1115::InvalidSample)
            {
                double solar_voltage_temp = solar_voltage_sample.voltage;
                solar_voltage_temp = solar_voltage_temp * (VOLTAGE_DIVIDER_A0_R1 + VOLTAGE_DIVIDER_A0_R2) / VOLTAGE_DIVIDER_A0_R2;
                StratoGlobals.solar_voltage = solar_voltage_temp;
                StratoGlobals.solar_voltage_mean = ((StratoGlobals.solar_voltage_mean * StratoGlobals.solar_voltage_count) + solar_voltage_temp) / (StratoGlobals.solar_voltage_count + 1);
                StratoGlobals.solar_voltage_count++;
                writeLogfile("solar_voltage_ads1115_ch1", timestamp_filename, timestamp_value, &solar_voltage_temp, 1);
            }
            else
                ads1115_inited = init_ads1115();

            ADS1115::Sample uv_guvas12sd_sample = StratoADS1115().getSample(ADS1115::CH2);
            if (uv_guvas12sd_sample != ADS1115::InvalidSample)
            {
                double uv_guvas12sd_temp = uv_guvas12sd_sample.voltage;
                // StratoGlobals.uv_guvas12sd = uv_guvas12sd_temp;
                // StratoGlobals.uv_guvas12sd_mean = ((StratoGlobals.uv_guvas12sd_mean * StratoGlobals.uv_guvas12sd_count) + uv_guvas12sd_temp) / (StratoGlobals.uv_guvas12sd_count + 1);
                // StratoGlobals.uv_guvas12sd_count++;
                writeLogfile("uv_guvas12sd_ads1115_ch2", timestamp_filename, timestamp_value, &uv_guvas12sd_temp, 1);
            }
            else
                ads1115_inited = init_ads1115();
        }
        else
            ads1115_inited = init_ads1115();
        #endif



        #ifdef QMC5883_ADDR
        if (qmc5883_inited)
        {
            double magnetXYZ_temp[3] {0};
            if (StratoQMC5883().getMagneticFieldXYZ(magnetXYZ_temp))
            {
                // for(uint8_t i{0}; i < 3; i++)
                //     StratoGlobals.magnetXYZ[i] = magnetXYZ_temp[i];
                // StratoGlobals.magnet = std::sqrt(magnetXYZ_temp[0] * magnetXYZ_temp[0] + magnetXYZ_temp[1] * magnetXYZ_temp[1] + magnetXYZ_temp[2] * magnetXYZ_temp[2]);
                // StratoGlobals.magnet_mean = ((StratoGlobals.magnet_mean * StratoGlobals.magnet_count) + StratoGlobals.magnet) / (StratoGlobals.magnet_count + 1);
                // StratoGlobals.magnet_count++;

                double temperature_temp = 0;
                if (StratoQMC5883().getTemperature(temperature_temp))
                {
                    double magnet_temperature_temp[4] {0};
                    for(uint8_t i{0}; i < 3; i++)
                        magnet_temperature_temp[i] = magnetXYZ_temp[i];
                    magnet_temperature_temp[3] = temperature_temp;
                    writeLogfile("magnetic_field_qmc5883", timestamp_filename, timestamp_value, magnet_temperature_temp, 4);
                    // std::cout << "getMagnet: " << magnet_temperature_temp[0] << " " << magnet_temperature_temp[1] <<  " " << magnet_temperature_temp[2] << " " << magnet_temperature_temp[3] << std::endl;
                }
                else
                {
                    writeLogfile("magnetic_field_qmc5883", timestamp_filename, timestamp_value, magnetXYZ_temp, 3);
                }
            }
            else
                qmc5883_inited = init_qmc5883();
        }
        else
            qmc5883_inited = init_qmc5883();
        #endif



        #ifdef LTR390UV01_ADDR
        if (ltr390uv01_inited)
        {
            LTR390UV01::Status status = StratoLTR390UV01().mainStatus();
            if (status.dataStatus)
            {
                double uv_temp = StratoLTR390UV01().read();
                // StratoGlobals.ltr390_uv = uv_temp;
                // StratoGlobals.ltr390_uv_mean = ((StratoGlobals.ltr390_uv_mean * StratoGlobals.ltr390_uv_count) + uv_temp) / (StratoGlobals.ltr390_uv_count + 1);
                // StratoGlobals.ltr390_uv_count++;
                writeLogfile("uv_ltr390", timestamp_filename, timestamp_value, &uv_temp, 1);
                // std::cout << "getLTR390UV: " << uv_temp << std::endl;
            }
            else
                ltr390uv01_inited = init_ltr390uv01;
        }
        else
                ltr390uv01_inited = init_ltr390uv01;
        #endif



        #ifdef AS7331_ADDR
        if (as7331_inited)
        {
            StratoAS7331().startMeasurement();

            AS7331::Status status = StratoAS7331().opStatus();
            for (uint8_t i{0}; i < 100 && !status.nData; i++)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                status = StratoAS7331().opStatus();
            }

            auto uva = StratoAS7331().readUVA();
            auto uvb = StratoAS7331().readUVB();
            auto uvc = StratoAS7331().readUVC();

            if (status.nData && uva.has_value() && uvb.has_value() && uvc.has_value())
            {
                double as7331_uv_temp[AS7331_UV_CHANNELS] {uva.value(), uvb.value(), uvc.value()};

                // for(uint8_t i{0}; i < AS7331_UV_CHANNELS; i++)
                // {
                //     StratoGlobals.as7331_uv[i] = as7331_uv_temp[i];
                //     StratoGlobals.as7331_uv_mean[i] = ((StratoGlobals.as7331_uv_mean[i] * StratoGlobals.as7331_uv_count) + as7331_uv_temp[i]) / (StratoGlobals.as7331_uv_count + 1);
                // }
                // StratoGlobals.as7331_uv_count++;
                writeLogfile("uv_as7331", timestamp_filename, timestamp_value, as7331_uv_temp, AS7331_UV_CHANNELS);
                // std::cout << "getAS7331UV: " << as7331_uv_temp[0] << " " << as7331_uv_temp[1] << " " << as7331_uv_temp[2] << std::endl;
            }
            else
                as7331_inited = init_as7331();
        }
        else
            as7331_inited = init_as7331();
        #endif



        #ifdef AS7343_ADDR
        if (as7343_inited)
        {
            auto spectrum_values = StratoAS7343().readSpectrum();
            if (spectrum_values.size() >= AS7343_SPECTRUM_CHANNELS)
            {
                double spectrum_temp[AS7343_SPECTRUM_CHANNELS] {0};
                for(uint8_t i{0}; i < AS7343_SPECTRUM_CHANNELS; i++)
                {
                    spectrum_temp[i] = spectrum_values.at(i).value;
                //     StratoGlobals.as7343_spectrum[i] = spectrum_temp[i];
                //     StratoGlobals.as7343_spectrum_mean[i] = ((StratoGlobals.as7343_spectrum_mean[i] * StratoGlobals.as7343_spectrum_count) + spectrum_temp[i]) / (StratoGlobals.as7343_spectrum_count + 1);
                }

                // StratoGlobals.as7343_spectrum_count++;
                writeLogfile("spectrum_as7343", timestamp_filename, timestamp_value, spectrum_temp, AS7343_SPECTRUM_CHANNELS);
                // std::cout << "getAS7343Spectrum: " << spectrum_temp[0] << " ..." << std::endl;
            }
            else
                as7343_inited = init_as7343();
        }
        else
            as7343_inited = init_as7343();
        #endif



        #ifdef BME280_ADDR
        if (bme280_inited)
        {
            TPH tph_temp {0};
            tph_temp = StratoBME280().getTPHValues();


            if (tph_temp.T > -141.0)
            {
                StratoGlobals.temperature = tph_temp.T;
                StratoGlobals.temperature_mean = ((StratoGlobals.temperature_mean * StratoGlobals.temperature_count) + tph_temp.T) / (StratoGlobals.temperature_count + 1);
                StratoGlobals.temperature_count++;
                writeLogfile("temperature_bme280", timestamp_filename, timestamp_value, &tph_temp.T, 1);
                // std::cout << "getTemperature: " << tph_temp.T << std::endl;
            }
            else
                bme280_inited = init_bme280();

            if (tph_temp.P > -999.0)
            {
                StratoGlobals.pressure = tph_temp.P;
                StratoGlobals.pressure_mean = ((StratoGlobals.pressure_mean * StratoGlobals.pressure_count) + tph_temp.P) / (StratoGlobals.pressure_count + 1);
                StratoGlobals.pressure_count++;
                writeLogfile("pressure_bme280", timestamp_filename, timestamp_value, &tph_temp.P, 1);
                // std::cout << "getPressure: " << tph_temp.P << std::endl;
            }
            else
                bme280_inited = init_bme280();

            if (tph_temp.H > -999.0)
            {
                // StratoGlobals.humidity = tph_temp.H;
                // StratoGlobals.humidity_mean = ((StratoGlobals.humidity_mean * StratoGlobals.humidity_count) + tph_temp.H) / (StratoGlobals.humidity_count + 1);
                // StratoGlobals.humidity_count++;
                writeLogfile("humidity_bme280", timestamp_filename, timestamp_value, &tph_temp.H, 1);
                // std::cout << "getHumidity: " << tph_temp.H << std::endl;
            }
            else
                bme280_inited = init_bme280();
        }
        else
            bme280_inited = init_bme280();
        #endif



        #ifdef SHT31_ADDR
        if (sht31_inited)
        {
            float temperature_temp_float = 0.0F;
            float humidity_temp_float = 0.0F;

            if (StratoSHT31().getValues(temperature_temp_float, humidity_temp_float))
            {
                double temperature_temp = temperature_temp_float;
                double humidity_temp = humidity_temp_float;

                // StratoGlobals.temperature = temperature_temp;
                // StratoGlobals.temperature_mean = ((StratoGlobals.temperature_mean * StratoGlobals.temperature_count) + temperature_temp) / (StratoGlobals.temperature_count + 1);
                // StratoGlobals.temperature_count++;
                writeLogfile("temperature_sht31", timestamp_filename, timestamp_value, &temperature_temp, 1);
                // std::cout << "getSHT31Temperature: " << temperature_temp << std::endl;

                // StratoGlobals.humidity = humidity_temp;
                // StratoGlobals.humidity_mean = ((StratoGlobals.humidity_mean * StratoGlobals.humidity_count) + humidity_temp) / (StratoGlobals.humidity_count + 1);
                // StratoGlobals.humidity_count++;
                writeLogfile("humidity_sht31", timestamp_filename, timestamp_value, &humidity_temp, 1);
                // std::cout << "getSHT31Humidity: " << humidity_temp << std::endl;
            }
            else
                sht31_inited = init_sht31();
        }
        else
            sht31_inited = init_sht31();
        #endif



        #ifdef MPU6050_ADDR
        if (mpu6050_inited)
        {
            MPU6050::Measurement measurement{};
            if (StratoMPU6050().getMeasurement(measurement) && measurement.valid)
            {
                double motion_temp[7] {
                    measurement.accelerationG.x,
                    measurement.accelerationG.y,
                    measurement.accelerationG.z,
                    measurement.gyroscopeDps.x,
                    measurement.gyroscopeDps.y,
                    measurement.gyroscopeDps.z,
                    measurement.temperatureC
                };

                // for(uint8_t i{0}; i < MPU6050_VECTOR_CHANNELS; i++)
                // {
                    // StratoGlobals.acceleration[i] = motion_temp[i];
                    // StratoGlobals.acceleration_mean[i] = ((StratoGlobals.acceleration_mean[i] * StratoGlobals.mpu6050_count) + motion_temp[i]) / (StratoGlobals.mpu6050_count + 1);

                //     const uint8_t gyro_index = i + MPU6050_VECTOR_CHANNELS;
                //     StratoGlobals.gyroscope[i] = motion_temp[gyro_index];
                //     StratoGlobals.gyroscope_mean[i] = ((StratoGlobals.gyroscope_mean[i] * StratoGlobals.mpu6050_count) + motion_temp[gyro_index]) / (StratoGlobals.mpu6050_count + 1);
                // }

                // StratoGlobals.mpu6050_temperature = measurement.temperatureC;
                // StratoGlobals.mpu6050_temperature_mean = ((StratoGlobals.mpu6050_temperature_mean * StratoGlobals.mpu6050_count) + measurement.temperatureC) / (StratoGlobals.mpu6050_count + 1);
                // StratoGlobals.mpu6050_count++;
                writeLogfile("motion_mpu6050", timestamp_filename, timestamp_value, motion_temp, 7);
                // std::cout << "getMPU6050Motion: " << motion_temp[0] << " " << motion_temp[1] << " " << motion_temp[2] << std::endl;
            }
            else
                mpu6050_inited = init_mpu6050();
        }
        else
            mpu6050_inited = init_mpu6050();
        #endif



        #ifdef OZONE3CLICK_LMP_ADDR
        #ifdef OZONE3CLICK_ADC_ADDR
        if (ozone3click_inited)
        {
            double ozone_temp = 0;
            if (StratoOZONE3CLICK().getOzone(ozone_temp))
            {
                // StratoGlobals.ozone = ozone_temp;
                // StratoGlobals.ozone_mean = (StratoGlobals.ozone_mean * StratoGlobals.ozone_count) + ozone_temp / (StratoGlobals.ozone_count + 1);
                // StratoGlobals.ozone_count++;
                writeLogfile("ozone_ozone3click", timestamp_filename, timestamp_value, &ozone_temp, 1);
                // std::cout << "getOzone: " << ozone_temp << std::endl;
            }
            else
                ozone3click_inited = init_ozone3click();
        }
        else
            ozone3click_inited = init_ozone3click();
        #endif
        #endif



        active = false;
        return true;
    }
    catch(...)
    {
        std::cerr << "Sensors execute failed" << std::endl;
        inited = false;
        active = false;
        return false;
    }
}
