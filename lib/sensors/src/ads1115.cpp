#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "ads1115.h"



#define ADS1115_DATA_REG        0x00
#define ADS1115_CONFIG_REG      0x01



ADS1115::ADS1115(uint8_t i2c_address)
    : i2cDevice(i2c_address), _address(i2c_address)
{}


bool ADS1115::init()
{
    OS = 0b0;
    MUX = 0b100;
    PGA = 0b001;
    MODE = 0b0;
    DR = 0b100;
    COMP_MODE = 0b0;
    COMP_POL = 0b0;
    COMP_LAT = 0b0;
    COMP_QUE = 0b11;

    bool success = setConfig();
    if (success)
        std::cout << "ADS1115 INITED" << std::endl;
    return success;
}


bool ADS1115::setConfig()
{
    uint16_t config =
        OS << 15        |   // Bit 15 No Single Conversion
        MUX << 12       |   // Bit 14:12 AIN0 + (channel << 12)) |    // MUX: AINx gegen GND
        PGA << 9        |   // Bit 11:9  PGA ±2.048 V
        MODE << 8       |   // Bit 8 Continous Mode
        DR << 5         |   // Bit 7:5 Data Rate
        COMP_MODE << 4  |   // Bit 4 Traditional Comparator
        COMP_POL << 3   |   // Bit 3 Comparator Active Low
        COMP_LAT << 2   |   // Bit 2 Nonlatching Comparator
        COMP_QUE;           // Bit 1:0 Comparator disabled
    uint8_t buf[2];
    buf[0] = config >> 8;
    buf[1] = config & 0xFF;

    if (!writeReg(ADS1115_CONFIG_REG, buf, 2))
    {
        std::cout << "ADS1115 SET CONFIG FAILED" << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}


bool ADS1115::setChannel(uint8_t channel)
{
    if(channel > 3)
        return false;
    MUX = 0b100 | channel;
    return setConfig();
}


bool ADS1115::getVoltageRawValue(int16_t &value)
{
    uint8_t buf[2];
    if (!readReg(ADS1115_DATA_REG, buf, 2))
    {
        std::cout << "ADS1115 READ FAILED" << std::endl;
        return false;
    }

    value = (int16_t)((buf[0] << 8) | buf[1]);
    return true;
}


bool ADS1115::getVoltage(double &voltage)
{
    int16_t raw;
    double range;
    if (!getVoltageRawValue(raw))
        return false;
    
    switch (PGA)
    {
    case 0b000:
        range = 6.144;
        break;
    case 0b001:
        range = 4.096;
        break;
    case 0b010:
        range = 2.048;
        break;
    case 0b011:
        range = 1.024;
        break;
    case 0b100:
        range = 0.512;
        break;
    case 0b101:
        range = 0.256;
        break;
    case 0b110:
        range = 0.256;
        break;
    case 0b111:
        range = 0.256;
        break;
    default:
        return false;
    }

    voltage = raw / 32768.0 * range;
    return true;
}
