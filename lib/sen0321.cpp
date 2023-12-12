#include "sen0321.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/*
* SEN0321 Ozone Sensor
*/

// S E N S O R   I M   M O M E N T   D E F E K T   ! ! !

#define MODE_REG 0x03
#define DATA_MSB_REG 0x09
#define DATA_LSB_REG 0x0A

bool SEN0321::init()
{
    std::cout << "SEN0321 INIT ..." << std::endl;
    
    uint8_t readBuf[1]; // 2 byte buffer to store the data read from the I2C device

    // init value 0 for gain
    // fGain = 0;

    // readBuf[0] = 0;

    int n;// = readReg(MODE_REG, readBuf, 1); // Read the id registers into readBuf

    // // if (fDebugLevel > 1)
    // // {
    // //     printf("%d bytes read\n", n);
    // //     printf("id reg: 0x%x \n", readBuf[0]);
    // // }

    // if (readBuf[0] != 0b11111111)   // das was im normalfall im id register stehen soll
    // {
    //     std::cout << "SEN0321 INIT FAILED" << std::endl;
    //     return false;
    // }

    // addr config reg A (CRA)
    // 8 average, 15 Hz, single measurement: 0x70

    uint8_t cmd = 0b00000000;     // das was ins cmd register rein soll

    n = writeReg(MODE_REG, &cmd, 1);

    std::cout << "SEN0321 INIT DONE" << std::endl;

    // setGain(fGain);
    return true;
}

// bool SEN0321::readRDYBit()
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
//         std::cout << "SEN0321 READY" << std::endl;
//         return true;
//     }
//     return false;
// }

bool SEN0321::getOzonRawValue(uint16_t& ozone)
{
    uint8_t readBuf[2];

    // uint8_t cmd = 0x01; // start single measurement
    // int n = writeReg(0x02, &cmd, 1); // addr mode reg (MR)
    // usleep(6000);

    // Read the 3 data registers into readBuf starting from addr 0x03
    int n = readReg(DATA_MSB_REG, readBuf, 2);
    uint16_t ozonereg = (uint16_t)(readBuf[0] << 8 | readBuf[1]);

    // if (fDebugLevel > 1) {
    //     printf("%d bytes read\n", n);
    //     printf("xreg: %d\n", xreg);
    //     printf("yreg: %d\n", yreg);
    //     printf("zreg: %d\n", zreg);
    // }

    ozone = ozonereg;

    if (ozonereg >= -2048 && ozonereg < 2048)
        return true;

    return false;
}

bool SEN0321::getOzone(double& ozone)
{
    uint16_t ozonereg;
    bool ok = getOzonRawValue(ozonereg);
    // double lsbgain = GAIN[fGain];
    ozone = /* lsbgain * */ ozonereg / 1000.;      // wrong factor?

    // if (fDebugLevel > 1) {
    //     printf("x field: %f G\n", x);
    //     printf("y field: %f G\n", y);
    //     printf("z field: %f G\n", z);
    // }

    return ok;
}
