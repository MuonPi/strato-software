#include "include/qmc5883.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/*
* QMC5883 3 axis magnetic field sensor
*/

#define DATA_X_LSB_REG 0x00
#define DATA_X_MSB_REG 0x01
#define DATA_Y_LSB_REG 0x02
#define DATA_Y_MSB_REG 0x03
#define DATA_Z_LSB_REG 0x04
#define DATA_Z_MSB_REG 0x05
#define STATUS_REG_A 0x06
#define TEMP_LSB_REG 0x07
#define TEMP_MSB_REG 0x08
#define CTL_REG_A 0x09
#define CTL_REG_B 0x0A
#define PERIOD_REG 0x0B
#define STATUS_REG_B 0x0C
#define ID_REG 0x0D

bool QMC5883::init()
{
    uint8_t readBuf[3]; // 2 byte buffer to store the data read from the I2C device

    // init value 0 for gain
    fGain = 0;

    readBuf[0] = 0;

    int n = readReg(ID_REG, readBuf, 1); // Read the id registers into readBuf

    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("id reg: 0x%x \n", readBuf[0]);
    }

    if (readBuf[0] != 0b11111111)   // das was im normalfall im id register stehen soll
        return false;

    // addr config reg A (CRA)
    // 8 average, 15 Hz, single measurement: 0x70
    uint8_t cmd1 = 0b00001101;     // das was ins cmd register rein soll
    uint8_t cmd2 = 0b00000001;
    uint8_t cmd3 = 0b00000001;
    n = writeReg(CTL_REG_A, &cmd1, 1);
    n = writeReg(CTL_REG_B, &cmd2, 1);
    n = writeReg(PERIOD_REG, &cmd3, 1);

    setGain(fGain);
    return true;
}

bool QMC5883::readRDYBit()
{
    uint8_t readBuf[3]; // 2 byte buffer to store the data read from the I2C device

    // addr status reg (SR)
    int n = readReg(STATUS_REG_A, readBuf, 1); // Read the status register into readBuf

    if (n != 1)
        return 0;
    uint8_t sr = readBuf[0];
    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("status (read from device): 0x%x\n", sr);
    }
    if ((sr & 0x01) == 0x01)
        return true;
    return false;
}

bool QMC5883::getXYZRawValues(int& x, int& y, int& z)
{
    uint8_t readBuf[6];

    // uint8_t cmd = 0x01; // start single measurement
    // int n = writeReg(0x02, &cmd, 1); // addr mode reg (MR)
    // usleep(6000);

    // Read the 3 data registers into readBuf starting from addr 0x03
    n = readReg(DATA_X_LSB_REG, readBuf, 6);
    int16_t xreg = (int16_t)(readBuf[0] | readBuf[1] << 8);
    int16_t yreg = (int16_t)(readBuf[2] | readBuf[3] << 8);
    int16_t zreg = (int16_t)(readBuf[4] | readBuf[5] << 8);

    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("xreg: %d\n", xreg);
        printf("yreg: %d\n", yreg);
        printf("zreg: %d\n", zreg);
    }

    x = xreg;
    y = yreg;
    z = zreg;

    if (xreg >= -2048 && xreg < 2048 && yreg >= -2048 && yreg < 2048 && zreg >= -2048 && zreg < 2048)
        return true;

    return false;
}

bool QMC5883::getXYZMagneticFields(double& x, double& y, double& z)
{
    int xreg, yreg, zreg;
    bool ok = getXYZRawValues(xreg, yreg, zreg);
    double lsbgain = GAIN[fGain];
    x = lsbgain * xreg / 1000.;
    y = lsbgain * yreg / 1000.;
    z = lsbgain * zreg / 1000.;

    if (fDebugLevel > 1) {
        printf("x field: %f G\n", x);
        printf("y field: %f G\n", y);
        printf("z field: %f G\n", z);
    }

    return ok;
}

bool QMC5883::getTemperatureRawValue(int& temperature)
{
    uint8_t readBuf[2];

    n = readReg(TEMP_LSB_REG, readBuf, 2);
    int16_t tempreg = (int16_t)(readBuf[0] | readBuf[1] << 8);

    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("tempreg: %d\n", tempreg);
    }

    temperature = tempreg;

    if (tempreg >= -2048 && tempreg < 2048)
        return true;

    return false;
}