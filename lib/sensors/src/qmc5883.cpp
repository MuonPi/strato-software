#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <cmath>

#include "qmc5883.h"

// /*
// * QMC5883 3 axis magnetic field sensor
// */

#define QMC5883_DATA_X_LSB_REG 0x00
#define QMC5883_DATA_X_MSB_REG 0x01
#define QMC5883_DATA_Y_LSB_REG 0x02
#define QMC5883_DATA_Y_MSB_REG 0x03
#define QMC5883_DATA_Z_LSB_REG 0x04
#define QMC5883_DATA_Z_MSB_REG 0x05
#define QMC5883_STATUS_REG_A 0x06
#define QMC5883_TEMP_LSB_REG 0x07
#define QMC5883_TEMP_MSB_REG 0x08
#define QMC5883_CTL_REG_A 0x09
#define QMC5883_CTL_REG_B 0x0A
#define QMC5883_PERIOD_REG 0x0B
#define QMC5883_STATUS_REG_B 0x0C
#define QMC5883_ID_REG 0x0D




QMC5883::QMC5883(uint8_t i2c_address)
    : i2cDevice(i2c_address), _address(i2c_address)
{}


bool QMC5883::init()
{
    OSR = 0b01;
    RNG = 0b00;
    ODR = 0b00;
    MODE = 0b01;
    SOFT_RST = 0b0;;
    ROL_PNT = 0b0;
    INT_ENB = 0b0;

    return setConfig();
}


bool QMC5883::setConfig()
{
    uint8_t config_a =
        OSR << 6        |
        RNG << 4        |
        ODR << 2        |
        MODE;
    uint8_t config_b =
        SOFT_RST << 7   |
        ROL_PNT << 6    |
        INT_ENB;

    if (!writeReg(QMC5883_CTL_REG_A, &config_a, 1))
    {
        std::cout << "QMC5883 SET CONFIG A FAILED" << std::endl;
        return false;
    }
    if (!writeReg(QMC5883_CTL_REG_B, &config_b, 1))
    {
        std::cout << "QMC5883 SET CONFIG B FAILED" << std::endl;
        return false;
    }
    return true;
}


bool QMC5883::getMagneticFieldRawValueXYZ(int16_t value[3])
{
    uint8_t buf[6];
    if(!readReg(QMC5883_DATA_X_LSB_REG, buf, 6))
    {
        std::cout << "QMC5883 READ FAILED" << std::endl;
        return false;
    }
    value[0] = (int16_t)((buf[1] << 8) | buf[0]);
    value[1] = (int16_t)((buf[3] << 8) | buf[2]);
    value[2] = (int16_t)((buf[5] << 8) | buf[4]);

    // if (xreg >= -2048 && xreg < 2048 && yreg >= -2048 && yreg < 2048 && zreg >= -2048 && zreg < 2048)
    //     return false;

    return true;
}


bool QMC5883::getMagneticFieldXYZ(double magnet[3])
{
    int16_t raw[3];
    double range;
    if(!getMagneticFieldRawValueXYZ(raw))
        return false;
    
    switch (RNG)
    {
    case 0b00:
        range = 2.0;
        break;
    case 0b01:
        range = 8.0;
        break;
    default:
        return false;
    }

    magnet[0] = raw[0] / 32768.0 * range;      // wrong factor?
    magnet[1] = raw[1] / 32768.0 * range;      // wrong factor?
    magnet[2] = raw[2] / 32768.0 * range;      // wrong factor?
    return true;
}


bool QMC5883::getMagneticField(double& magnet)
{
    double value[3];
    if(!getMagneticFieldXYZ(value))
        return false;
    
    magnet = std::sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2]);
    return true;
}


bool QMC5883::getTemperatureRawValue(int16_t& temperature)
{
    uint8_t buf[2];
    if(!readReg(QMC5883_TEMP_LSB_REG, buf, 2))
    {
        std::cout << "QMC5883 READ FAILED" << std::endl;
        return false;
    }
    temperature = (int16_t)((buf[1] << 8) | buf[0]);


    // if (tempreg >= -2048 && tempreg < 2048)
    //     return false;

    return true;
}


bool QMC5883::getTemperature(double& temperature)
{
    int16_t raw;
    if(!getTemperatureRawValue(raw))
        return false;
    
    temperature = raw / 100.;      // wrong factor?
    return true;
}













// bool QMC5883::init()
// {
//     std::cout << "QMC5883 INIT ..." << std::endl;
    
//     uint8_t readBuf[1]; // 2 byte buffer to store the data read from the I2C device

//     // init value 0 for gain
//     // fGain = 0;

//     // readBuf[0] = 0;

//     int n = readReg(ID_REG, readBuf, 1); // Read the id registers into readBuf

//     // if (fDebugLevel > 1)
//     // {
//     //     printf("%d bytes read\n", n);
//     //     printf("id reg: 0x%x \n", readBuf[0]);
//     // }

//     if (readBuf[0] != 0b11111111)   // das was im normalfall im id register stehen soll
//     {
//         std::cout << "QMC5883 INIT FAILED" << std::endl;
//         return false;
//     }

//     // addr config reg A (CRA)
//     // 8 average, 15 Hz, single measurement: 0x70

//     uint8_t cmd1 = 0b00001101;     // das was ins cmd register rein soll
//     uint8_t cmd2 = 0b00000001;
//     uint8_t cmd3 = 0b00000001;

//     n = writeReg(CTL_REG_A, &cmd1, 1);
//     n = writeReg(CTL_REG_B, &cmd2, 1);
//     n = writeReg(PERIOD_REG, &cmd3, 1);

//     std::cout << "QMC5883 INIT DONE" << std::endl;

//     // setGain(fGain);
//     return true;
// }

// bool QMC5883::readRDYBit()
// {
//     uint8_t readBuf[1]; // 2 byte buffer to store the data read from the I2C device

//     // addr status reg (SR)
//     int n = readReg(STATUS_REG_A, readBuf, 1); // Read the status register into readBuf

//     if (n != 1)
//         return false;
    
//     uint8_t sr = readBuf[0];

//     // if (fDebugLevel > 1)
//     // {
//     //     printf("%d bytes read\n", n);
//     //     printf("status (read from device): 0x%x\n", sr);
//     // }

//     if ((sr & 0b00000001) == 0b00000001)
//     {
//         std::cout << "QMC5883 READY" << std::endl;
//         return true;
//     }
//     return false;
// }





// bool QMC5883::getTemperatureRawValue(uint16_t& temperature)
// {
//     uint8_t readBuf[2];

//     int n = readReg(TEMP_LSB_REG, readBuf, 2);
//     uint16_t tempreg = (uint16_t)(readBuf[0] | readBuf[1] << 8);

//     // if (fDebugLevel > 1) {
//     //     printf("%d bytes read\n", n);
//     //     printf("tempreg: %d\n", tempreg);
//     // }

//     temperature = tempreg;

//     if (tempreg >= -2048 && tempreg < 2048)
//         return true;

//     return false;
// }

// bool QMC5883::getTemperature(double& temperature)
// {
//     uint16_t tempreg;
//     bool ok = getTemperatureRawValue(tempreg);
//     temperature = tempreg / 100.;      // wrong factor?

//     // if (fDebugLevel > 1) {
//     //     printf("x field: %f G\n", x);
//     //     printf("y field: %f G\n", y);
//     //     printf("z field: %f G\n", z);
//     // }

//     return ok;
// }
