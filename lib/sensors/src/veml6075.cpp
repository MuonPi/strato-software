
#include <iostream>
#include <unistd.h>
#include "veml6075.h"


// /*
// * VEML6075 uv sensor
// */

#define VEML6075_CONF_REG 0x00
#define VEML6075_DATA_UVA_REG 0x07
#define VEML6075_DATA_UVB_REG 0x09
#define VEML6075_DATA_UV_COMP1_REG 0x0A
#define VEML6075_DATA_UV_COMP2_REG 0x0B
#define VEML6075_ID_REG 0x0C


VEML6075::VEML6075(uint8_t i2c_address)
    : i2cDevice(i2c_address), _address(i2c_address)
{}


bool VEML6075::init()
{
    UV_IT = 0b000;  //0b000
    HD = 0b0;
    UV_TRIG = 0b0;
    UV_AF = 0b0;
    AD = 0b0;

    bool success = setConfig();
    if (success)
        std::cout << "VEML6075 inited" << std::endl;
    

    // uint8_t buf[1];
    // readReg(VEML6075_ID_REG, buf, 1);
    // std::cout << static_cast<int>(buf[0]) << std::endl;
    // uint8_t id[2];
    // readReg(VEML6075_ID_REG, id, 2);
    // std::cout << "ID: " << std::hex << ((id[1] << 8) | id[0]) << std::endl;


    return success;
}


bool VEML6075::setConfig()
{
    uint8_t config =
        UV_IT << 4      |
        HD << 3         |
        UV_TRIG << 2    |
        UV_AF << 1      |
        AD;
    uint8_t buf[2];
    buf[0] = config;
    buf[1] = 0x00;

    if (!writeReg(VEML6075_CONF_REG, buf, 2))
    {
        std::cerr << "VEML6075 SET CONFIG FAILED" << std::endl;
        return false;
    }
    return true;
}


bool VEML6075::getUVRawValue(int16_t* value)
{
    uint8_t buf[2];
    if(!readReg(VEML6075_DATA_UVA_REG, buf, 2))
    {
        std::cerr << "VEML6075 READ FAILED" << std::endl;
        return false;
    }
    value[0] = (int16_t)((buf[1] << 8) | buf[0]); // UVA
    if(!readReg(VEML6075_DATA_UVB_REG, buf, 2))
    {
        std::cerr << "VEML6075 READ FAILED" << std::endl;
        return false;
    }
    value[1] = (int16_t)((buf[1] << 8) | buf[0]); // UVB
    if(!readReg(VEML6075_DATA_UV_COMP1_REG, buf, 2))
    {
        std::cerr << "VEML6075 READ FAILED" << std::endl;
        return false;
    }
    value[2] = (int16_t)((buf[1] << 8) | buf[0]); // COMP1
    if(!readReg(VEML6075_DATA_UV_COMP2_REG, buf, 2))
    {
        std::cerr << "VEML6075 READ FAILED" << std::endl;
        return false;
    }
    value[3] = (int16_t)((buf[1] << 8) | buf[0]); // COMP2

    // std::cout << value[0] << std::endl;

    // if (xreg >= -2048 && xreg < 2048 && yreg >= -2048 && yreg < 2048 && zreg >= -2048 && zreg < 2048)
    //     return false;

    return true;
}


bool VEML6075::getUV(double* uv)
{
    int16_t raw[2];
    uint8_t factor;
    if(!getUVRawValue(raw))
        return false;
    
    switch (UV_IT)
    {
    case 0b000:
        factor = 1;
        break;
    case 0b001:
        factor = 2;
        break;
    case 0b010:
        factor = 4;
        break;
    case 0b011:
        factor = 8;
        break;
    case 0b100:
        factor = 16;
        break;
    default:
        return false;
    }

    uv[0] = (double)raw[0]; // / 32768.0 / factor;      // wrong factor?
    uv[1] = (double)raw[1]; // / 32768.0 / factor;      // wrong factor?
    return true;
}











