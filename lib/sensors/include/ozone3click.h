#ifndef _OZONE3CLICK_H_
#define _OZONE3CLICK_H_


#include <stdint.h>
#include "i2cdevice.h"

class OZONE3CLICK : public i2cDevice
{
public:
    OZONE3CLICK(uint8_t i2c_address);

    bool init();
    bool setConfig();
    bool getOzoneRawValue(int16_t& temperature);
    bool getOzone(double& temperature);

    // uint8_t OSR = 0b00;
    // uint8_t RNG = 0b00;
    // uint8_t ODR = 0b00;
    // uint8_t MODE = 0b00;
    // uint8_t SOFT_RST = 0b0;
    // uint8_t ROL_PNT = 0b0;
    // uint8_t INT_ENB = 0b0;

private:
    uint8_t _address;
};

#endif // _QMC5883_H_


#endif // _OZONE3CLICK_H_