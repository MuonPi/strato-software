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
    bool getXYZRawValues(int16_t& x, int16_t& y, int16_t& z);
    bool getXYZMagneticFields(double& x, double& y, double& z);
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

// class QMC5883 : public i2cDevice
// {
// public:
//     // Resolution for the 8 gain settings in mG/LSB
//     static const double GAIN[8];
//     QMC5883()
//         : i2cDevice(0x0d)
//     {
//         fTitle = "QMC5883";
//     }
//     QMC5883(const char* busAddress, uint8_t slaveAddress)
//         : i2cDevice(busAddress, slaveAddress)
//     {
//         fTitle = "QMC5883";
//     }
//     QMC5883(uint8_t slaveAddress)
//         : i2cDevice(slaveAddress)
//     {
//         fTitle = "QMC5883";
//     }

//     bool init();
//     bool readRDYBit();
//     // gain range 0..7
//     // void setGain(uint8_t gain);
//     // uint8_t readGain();
//     bool getXYZRawValues(uint16_t& x, uint16_t& y, uint16_t& z);
//     bool getXYZMagneticFields(double& x, double& y, double& z);
//     bool getTemperatureRawValue(uint16_t& temperature);
//     bool getTemperature(double& temperature);
//     // bool readLockBit();
//     // bool calibrate(int& x, int& y, int& z);

// // private:
// //     unsigned int fLastConvTime;
// //     bool fCalibrationValid;
// //     unsigned int fGain;
// //     signed int fCalibParameters[11];
// };

#endif // !_QMC5883_H_
