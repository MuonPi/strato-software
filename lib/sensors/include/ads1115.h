#ifndef _ADS1115_H_
#define _ADS1115_H_

// #include <chrono>
// #include <functional>
// #include <mutex>
// #include <thread>

// #include "hardware/device_types.h"


#include <stdint.h>
#include "i2cdevice.h"

class ADS1115 : public i2cDevice
{
public:
    ADS1115(uint8_t i2c_address);

    bool init();
    // bool getConfig(uint16_t &value);
    bool setConfig();
    bool setChannel(uint8_t channel);
    bool getVoltageRawValue(int16_t &value);
    bool getVoltage(double &voltage);

    uint8_t OS = 0b0;
    uint8_t MUX = 0b000;
    uint8_t PGA = 0b010;
    uint8_t MODE = 0b1;
    uint8_t DR = 0b100;
    uint8_t COMP_MODE = 0b0;;
    uint8_t COMP_POL = 0b0;
    uint8_t COMP_LAT = 0b0;
    uint8_t COMP_QUE = 0b11;

private:
    uint8_t _address;
};

#endif // !_ADS1115_H_
