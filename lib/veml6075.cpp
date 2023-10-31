#include "include/veml6075.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/*
* VEML6075 UV Sensor
*/

bool VEML6075::init()
{
    uint8_t readBuf[3]; // 2 byte buffer to store the data read from the I2C device

    // init value 0 for gain
    fGain = 0;

    readBuf[0] = 0;
    readBuf[1] = 0;
    readBuf[2] = 0;

    int n = readReg(0x0a, readBuf, 3); // Read the id registers into readBuf

    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("id reg A: 0x%x \n", readBuf[0]);
        printf("id reg B: 0x%x \n", readBuf[1]);
        printf("id reg C: 0x%x \n", readBuf[2]);
    }

    if (readBuf[0] != 0x48)
        return false;

    // addr config reg A (CRA)
    // 8 average, 15 Hz, single measurement: 0x70
    uint8_t cmd = 0x70;
    n = writeReg(0x00, &cmd, 1);

    setGain(fGain);
    return true;
}
