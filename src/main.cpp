#include "i2cdevice.h"
// #include "i2cdevice.cpp"
// #include "ads1115.h"
// #include "bme280.h"
#include "qmc5883.h"
// #include "veml6075.h"



int main()
{
    QMC5883 Sensor_QMC5883;
    Sensor_QMC5883.init();
    
}