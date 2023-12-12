#include <thread>
#include <chrono>
#include "i2cdevice.h"
#include "sen0321.h"
// #include "ads1115.h"
// #include "bme280.h"
#include "qmc5883.h"
#include "veml6075.h"

// uint16_t x_raw, y_raw, z_raw, temperature_raw;
// double x_real, y_real, z_real, temperature_real;

// uint16_t ozone_raw;

uint16_t uva_raw, uvb_raw;

int main()
{
    // QMC5883 Sensor_QMC5883;
    // Sensor_QMC5883.init();
    // Sensor_QMC5883.readRDYBit();
    // Sensor_QMC5883.getXYZRawValues(x_raw, y_raw, z_raw);
    // std::cout << "XYZ raw: " << x_raw << " " << y_raw << " "  << z_raw << std::endl;
    // Sensor_QMC5883.getXYZMagneticFields(x_real, y_real, z_real);
    // std::cout << "XYZ real: " << x_real << " " << y_real << " "  << z_real << std::endl;
    // Sensor_QMC5883.getTemperatureRawValue(temperature_raw);
    // std::cout << "Temperature raw: " << temperature_raw << std::endl;
    // Sensor_QMC5883.getTemperature(temperature_real);
    // std::cout << "Temperature real: " << temperature_real << std::endl;




    // SEN0321 Sensor_SEN0321;
    // Sensor_SEN0321.init();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // for(int i; i < 100; i++)
    // {
    //     Sensor_SEN0321.getOzonRawValue(ozone_raw);
    //     std::cout << "Ozone raw: " << ozone_raw << std::endl;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }




    VEML6075 Sensor_VEML6075;
    Sensor_VEML6075.init();
    Sensor_VEML6075.getUVRawValue(uva_raw, uvb_raw);
    std::cout << "UV raw: " << uva_raw << " " << uvb_raw << std::endl;
}
