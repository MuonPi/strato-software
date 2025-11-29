#ifndef _VEML6075_H_
#define _VEML6075_H_

#include "i2cdevice.h"

// /* VEML6075 */


class VEML6075 : public i2cDevice {
public:
    VEML6075(uint8_t address = 0x10);

    bool init();
    bool getUVRaw(uint16_t& uva, uint16_t& uvb);

private:
    static constexpr uint8_t REG_CONF = 0x00;
    static constexpr uint8_t REG_UVA  = 0x07;
    static constexpr uint8_t REG_UVB  = 0x09;
};








// class VEML6075 : public i2cDevice {
// public:
//     explicit VEML6075(int bus, int address);

//     bool init();
//     bool getUVRawValue(uint16_t& uva, uint16_t& uvb);

// private:
//     static constexpr uint8_t REG_CONF = 0x00;
//     static constexpr uint8_t REG_UVA  = 0x07;
//     static constexpr uint8_t REG_UVB  = 0x09;
// };





// class VEML6075 : public i2cDevice
// {
// public:
//     // Resolution for the 8 gain settings in mG/LSB
//     VEML6075()
//         : i2cDevice(0x10)
//     {
//         fTitle = "VEML6075";
//     }
//     VEML6075(const char* busAddress, uint8_t slaveAddress)
//         : i2cDevice(busAddress, slaveAddress)
//     {
//         fTitle = "VEML6075";
//     }
//     VEML6075(uint8_t slaveAddress)
//         : i2cDevice(slaveAddress)
//     {
//         fTitle = "VEML6075";
//     }

//     bool init();
//     // bool readRDYBit();
//     // gain range 0..7
//     // void setGain(uint8_t gain);
//     // uint8_t readGain();
//     bool getUVRawValue(uint16_t& uva, uint16_t& uvb);
//     // bool getUV(double& uva, double& uvb);
//     // bool readLockBit();
//     // bool calibrate(int& x, int& y, int& z);

// // private:
// //     unsigned int fLastConvTime;
// //     bool fCalibrationValid;
// //     unsigned int fGain;
// //     signed int fCalibParameters[11];
// };

#endif // !_VEML6075_H_
