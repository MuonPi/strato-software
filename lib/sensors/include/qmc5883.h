#ifndef _QMC5883_H_
#define _QMC5883_H_


#include <stdint.h>
#include "i2cdevice.h"

class QMC5883 : public i2cDevice
{
public:
    QMC5883(uint8_t i2c_address);

    bool init();
    // bool getConfig(uint16_t &value);
    bool setConfig();
    bool getMagneticFieldRawValueXYZ(int16_t* value);
    bool getMagneticFieldXYZ(double* magnet);
    bool getMagneticField(double& magnet);
    bool getTemperatureRawValue(int16_t& temperature);
    bool getTemperature(double& temperature);

    uint8_t OSR = 0b00;
    uint8_t RNG = 0b00;
    uint8_t ODR = 0b00;
    uint8_t MODE = 0b00;
    uint8_t SOFT_RST = 0b0;;
    uint8_t ROL_PNT = 0b0;
    uint8_t INT_ENB = 0b0;

private:
    uint8_t _address;
};

#endif // _QMC5883_H_
