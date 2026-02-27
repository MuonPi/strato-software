#ifndef _SEN0321_H_
#define _SEN0321_H_

#include "i2cdevice.h"

// /* SEN0321 */

// class SEN0321 : public i2cDevice
// {
// public:
//     // Resolution for the 8 gain settings in mG/LSB
//     static const double GAIN[8];
//     SEN0321()
//         : i2cDevice(0x73)
//     {
//         fTitle = "SEN0321";
//     }
//     SEN0321(const char* busAddress, uint8_t slaveAddress)
//         : i2cDevice(busAddress, slaveAddress)
//     {
//         fTitle = "SEN0321";
//     }
//     SEN0321(uint8_t slaveAddress)
//         : i2cDevice(slaveAddress)
//     {
//         fTitle = "SEN0321";
//     }

//     bool init();
//     // bool readRDYBit();
//     // gain range 0..7
//     // void setGain(uint8_t gain);
//     // uint8_t readGain();
//     bool getOzonRawValue(uint16_t& ozone);
//     bool getOzone(double& ozone);
//     // bool readLockBit();
//     // bool calibrate(int& x, int& y, int& z);

// // private:
// //     unsigned int fLastConvTime;
// //     bool fCalibrationValid;
// //     unsigned int fGain;
// //     signed int fCalibParameters[11];
// };

#endif // !_SEN0321_H_
