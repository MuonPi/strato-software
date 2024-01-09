#include "veml6075.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <thread>


/*
* VEML6075 uv sensor
*/

#define CTL_REG 0x00
#define DATA_REG_UVA 0x07
#define DATA_REG_UVB 0x09
#define ID_REG 0x0C

bool VEML6075::init()
{
    int n;
    std::cout << "VEML6075 INIT ..." << std::endl;
    
    uint8_t readBuf[4]; // 2 byte buffer to store the data read from the I2C device
    std::cout << "rückgabewert read: ";
    for (auto i = 0u; i<4; i++){
        n = readBuf[i] = 0;
        std::cout << n << " ";
    }
    std::cout << std::endl;

    readReg(CTL_REG, readBuf, 4);
    std::cout << "ausgelesene Werte: ";
    for (auto i = 0u; i < 4; i++){
        std::cout << std::hex << static_cast<int>(readBuf[i]) << " ";
        std::cout << std::hex << static_cast<unsigned>(readBuf[i]) << "  ";
    }
    std::cout << std::endl;


    // uint8_t reg[2];
    // reg[0] = CTL_REG;
    // // reg[1] = 0x10;
    // write(reg, 1);
    // std::this_thread::sleep_for(std::chrono::microseconds(1000));
    // read(readBuf, 4);

    // int n = readWord(ID_REG, readBuf); // Read the id registers into readBuf

    // readBuf[1] = 11;

    // printf("%d", &readBuf[0]);
    // std::cout << static_cast<int>(readBuf[0]) << "  " << static_cast<int>(readBuf[1]) << std::endl;

    // if (fDebugLevel > 1)
    // {
    //     printf("%d bytes read\n", n);
    //     printf("id reg: 0x%x \n", readBuf[0]);
    // }

    // if (readBuf[0] != 0b01001000)   // das was im normalfall im id register stehen soll
    // {
    //     std::cout << "VEML6075 INIT FAILED" << std::endl;
    //     return false;
    // }

    // addr config reg A (CRA)
    // 8 average, 15 Hz, single measurement: 0x70

    uint8_t cmd[2];
    cmd[0] = 0b01001000;     // das was ins cmd register rein soll
    cmd[1] = 0b01001000;

    n = writeReg(CTL_REG, cmd, 2);
    std::cout << "rückgabewert write: ";
    std::cout << n << " " << std::endl;

    // int n = readReg(ID_REG, readBuf, 4); // Read the id registers into readBuf

    // for (auto i = 0u; i < 4; i++){
    //     std::cout << std::hex << static_cast<unsigned>(readBuf[i]) << ' ';
    // }

    

    // std::cout << (int)(readBuf[0]) << (int)(readBuf[1]) << std::endl;

    // std::cout << "VEML6075 INIT DONE" << std::endl;

    // setGain(fGain);
    return true;
}

// bool VEML6075::readRDYBit()
// {
//     // uint8_t readBuf[1]; // 2 byte buffer to store the data read from the I2C device

//     // // addr status reg (SR)
//     // int n = readReg(STATUS_REG_A, readBuf, 1); // Read the status register into readBuf

//     // if (n != 1)
//     //     return false;
    
//     // uint8_t sr = readBuf[0];

//     // // if (fDebugLevel > 1)
//     // // {
//     // //     printf("%d bytes read\n", n);
//     // //     printf("status (read from device): 0x%x\n", sr);
//     // // }

//     // if ((sr & 0b00000001) == 0b00000001)
//     // {
//     //     std::cout << "VEML6075 READY" << std::endl;
//     //     return true;
//     // }
//     // return false;
// }

bool VEML6075::getUVRawValue(uint16_t& uva, uint16_t& uvb)
{
    uint8_t readBuf[2];

    // uint8_t cmd = 0x01; // start single measurement
    // int n = writeReg(0x02, &cmd, 1); // addr mode reg (MR)
    // usleep(6000);

    // Read the 3 data registers into readBuf starting from addr 0x03
    int n = readReg(DATA_REG_UVA, readBuf, 2);
    uint16_t uvareg = (uint8_t)(readBuf[0] << 8 | (uint8_t)(readBuf[1]));
    n = readReg(DATA_REG_UVB, readBuf, 2);
    uint16_t uvbreg = (uint8_t)(readBuf[0] << 8 | (uint8_t)(readBuf[1]));

    // if (fDebugLevel > 1) {
    //     printf("%d bytes read\n", n);
    //     printf("xreg: %d\n", xreg);
    //     printf("yreg: %d\n", yreg);
    //     printf("zreg: %d\n", zreg);
    // }

    uva = uvareg;
    uvb = uvbreg;

    if (uvareg >= -2048 && uvareg < 2048 && uvbreg >= -2048 && uvbreg < 2048)
        return true;

    return false;
}

// bool VEML6075::getXYZMagneticFields(double& x, double& y, double& z)
// {
//     uint16_t xreg, yreg, zreg;
//     bool ok = getXYZRawValues(xreg, yreg, zreg);
//     // double lsbgain = GAIN[fGain];
//     x = /* lsbgain * */ xreg / 1000.;      // wrong factor?
//     y = /* lsbgain * */ yreg / 1000.;      // wrong factor?
//     z = /* lsbgain * */ zreg / 1000.;      // wrong factor?

//     // if (fDebugLevel > 1) {
//     //     printf("x field: %f G\n", x);
//     //     printf("y field: %f G\n", y);
//     //     printf("z field: %f G\n", z);
//     // }

//     return ok;
// }

// bool VEML6075::getTemperatureRawValue(uint16_t& temperature)
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

// bool VEML6075::getTemperature(double& temperature)
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
