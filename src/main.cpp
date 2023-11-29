#include "i2cdevice.h"
// #include "i2cdevice.cpp"
// #include "ads1115.h"
// #include "bme280.h"
#include "qmc5883.h"
// #include "veml6075.h"

uint16_t x_raw, y_raw, z_raw, temperature_raw;

double x_real, y_real, z_real, temperature_real;

int main()
{
    QMC5883 Sensor_QMC5883;
    Sensor_QMC5883.init();
    Sensor_QMC5883.readRDYBit();
    Sensor_QMC5883.getXYZRawValues(x_raw, y_raw, z_raw);
    std::cout << "XYZ raw: " << x_raw << " " << y_raw << " "  << z_raw << std::endl;
    Sensor_QMC5883.getXYZMagneticFields(x_real, y_real, z_real);
    std::cout << "XYZ real: " << x_real << " " << y_real << " "  << z_real << std::endl;
    Sensor_QMC5883.getTemperatureRawValue(temperature_raw);
    std::cout << "Temperature raw: " << temperature_raw << std::endl;
    Sensor_QMC5883.getTemperature(temperature_real);
    std::cout << "Temperature real: " << temperature_real << std::endl;
}
