#ifndef _QMC5883_H_
#define _QMC5883_H_

#include "i2cdevice.h"

/* QMC5883  */

class QMC5883 : public i2cDevice {
public:
    // Resolution for the 8 gain settings in mG/LSB
    static const double GAIN[8];
    QMC5883()
        : i2cDevice(0x1e)
    {
        fTitle = "QMC5883";
    }
    QMC5883(const char* busAddress, uint8_t slaveAddress)
        : i2cDevice(busAddress, slaveAddress)
    {
        fTitle = "QMC5883";
    }
    QMC5883(uint8_t slaveAddress)
        : i2cDevice(slaveAddress)
    {
        fTitle = "QMC5883";
    }

    bool init();
    // gain range 0..7
    void setGain(uint8_t gain);
    uint8_t readGain();
    bool getXYZRawValues(int& x, int& y, int& z);
    bool getXYZMagneticFields(double& x, double& y, double& z);
    bool readRDYBit();
    bool readLockBit();
    bool calibrate(int& x, int& y, int& z);

private:
    unsigned int fLastConvTime;
    bool fCalibrationValid;
    unsigned int fGain;
    signed int fCalibParameters[11];
};

#endif // !_QMC5883_H_
