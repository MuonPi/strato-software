#include <stdio.h>
#include <cstdint>
#include <iostream>
#include "bme280.h"


/*
*BME280 HumidityTemperaturePressuresensor
*
*/

bool BME280::init()
{
    fTitle = "BME280";

    std::uint8_t val = 0;
    fCalibrationValid = false;

    // chip id reg
    int n = readReg(0xd0, &val, 1); // Read the id register into readBuf

    if (fDebugLevel > 1) {
        printf("%d bytes read\n", n);
        printf("chip id: 0x%x \n", val);
    }

    std::uint8_t buf[1];
    buf[0] = 0xb6; // reset
    writeReg(0x0e, buf, 1);

    buf[0] = 0b00000101; // oversampling humidity
    writeReg(0xf2, buf, 1);

    buf[0] = 0b10110111; // oversampling temperature & humidity max (16x) mode normal
    writeReg(0xf4, buf, 1);

    if (val == 0x60)
    {
        readCalibParameters();
        std::cout << "BME280 inited" << std::endl;
    }
    else
    {
        std::cerr << "BME280 SET CONFIG FAILED" << std::endl;
    }
    return (val == 0x60);
}

TPH BME280::readTPCU()
{
    uint8_t readBuf[8];
    for (int i = 0; i < 8; i++) {
        readBuf[i] = 0;
    }

    TPH val;
    // if (!measure()) {
    //     std::cerr << "error: measurement invalid";
    //     val.adc_P = INT32_MIN;
    //     val.adc_T = INT32_MIN;
    //     val.adc_H = INT32_MIN;
    //     return val;
    // }

    int n = readReg(0xf7, readBuf, 8); // read T, P and H registers;
    if (fDebugLevel > 1)
        printf("%d bytes read\n", n);
    uint32_t adc_P = ((uint32_t)readBuf[0]) << 12;
    adc_P |= ((uint32_t)readBuf[1]) << 4;
    adc_P |= ((uint32_t)readBuf[2]) >> 4;
    uint32_t adc_T = ((uint32_t)readBuf[3]) << 12;
    adc_T |= ((uint32_t)readBuf[4]) << 4;
    adc_T |= ((uint32_t)readBuf[5]) >> 4;
    uint32_t adc_H = ((uint32_t)readBuf[6]) << 8;
    adc_H |= ((uint32_t)readBuf[7]); // (look datasheet page 25)

    val.adc_P = (int32_t)adc_P;
    val.adc_T = (int32_t)adc_T;
    val.adc_H = (int32_t)adc_H;
    return val;
}

// bool BME280::softReset()
// {
//     uint8_t resetWord[1];
//     resetWord[0] = 0xb6;
//     int val = writeReg(0xe0, resetWord, 1);
//     return (val == 1);
// }

// uint16_t BME280::getCalibParameter(unsigned int param) const
// {
//     if (param < 18)
//         return fCalibParameters[param];
//     return 0xffff;
// }

void BME280::readCalibParameters()
{
    std::uint8_t reg88[25];
    std::uint8_t regE1[07];
    // register address first byte eeprom
    int n = readReg(0x88, reg88, 25); // Read the 25 eeprom word values into readBuf
    n = n + readReg(0xe1, regE1, 7); // from two different locations


    std::uint16_t dig[] = {
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[1]) << 8) | static_cast<std::uint16_t>(reg88[0])),   // dig_T1
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[3]) << 8) | static_cast<std::uint16_t>(reg88[2])),   // dig_T2
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[5]) << 8) | static_cast<std::uint16_t>(reg88[4])),   // dig_T3
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[7]) << 8) | static_cast<std::uint16_t>(reg88[6])),   // dig_P1
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[9]) << 8) | static_cast<std::uint16_t>(reg88[8])),   // dig_P2
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[11]) << 8) | static_cast<std::uint16_t>(reg88[10])), // dig_P3
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[13]) << 8) | static_cast<std::uint16_t>(reg88[12])), // dig_P4
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[15]) << 8) | static_cast<std::uint16_t>(reg88[14])), // dig_P5
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[17]) << 8) | static_cast<std::uint16_t>(reg88[16])), // dig_P6
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[19]) << 8) | static_cast<std::uint16_t>(reg88[18])), // dig_P7
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[21]) << 8) | static_cast<std::uint16_t>(reg88[20])), // dig_P8
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(reg88[23]) << 8) | static_cast<std::uint16_t>(reg88[22])), // dig_P9

        static_cast<std::uint16_t>(reg88[25]),                                                   // dig_H1
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(regE1[1]) << 8) | static_cast<std::uint16_t>(regE1[0])),         // dig_H2
        static_cast<std::uint16_t>(regE1[2]),                                                                           // dig_H3
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(regE1[3]) << 4) | static_cast<std::uint16_t>(regE1[4] & 0x0Fu)), // dig_H4
        static_cast<std::uint16_t>((static_cast<std::uint16_t>(regE1[5]) << 4) | (static_cast<std::uint16_t>(regE1[4]) >> 4)),  // dig_H5
        static_cast<std::uint16_t>(regE1[6])};
    dig_T1 = static_cast<std::int32_t>(dig[0]);
    dig_T2 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[1]));
    dig_T3 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[2]));
    dig_P1 = static_cast<std::int32_t>(dig[3]);
    dig_P2 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[4]));
    dig_P3 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[5]));
    dig_P4 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[6]));
    dig_P5 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[7]));
    dig_P6 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[8]));
    dig_P7 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[9]));
    dig_P8 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[10]));
    dig_P9 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[11]));
    dig_H1 = static_cast<std::int32_t>(static_cast<std::uint8_t>(dig[12] & 0xffu));
    dig_H2 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[13]));
    dig_H3 = static_cast<std::int32_t>(static_cast<std::uint8_t>(dig[14] & 0xffu));
    dig_H4 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[15]));
    dig_H5 = static_cast<std::int32_t>(static_cast<std::int16_t>(dig[16]));
    dig_H6 = static_cast<std::int32_t>(static_cast<int8_t>(static_cast<std::uint8_t>(dig[17] & 0xffu)));

}

auto BME280::getTPHValues() -> TPH
{
    TPH vals = readTPCU();
    vals.T = BME280_compensate_T_int32(vals.adc_T)/100.;
    vals.P = BME280_compensate_P_int64(vals.adc_P)/256.;
    vals.H = BME280_compensate_H_int32(vals.adc_H)/1024.;
    return vals;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
auto BME280::BME280_compensate_T_int32(std::int32_t adc_T) -> std::int32_t
{
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
            ((int32_t)dig_T3)) >>
           14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
auto BME280::BME280_compensate_P_int64(std::int32_t adc_P) -> std::uint32_t
{
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (uint32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
auto BME280::BME280_compensate_H_int32(std::int32_t adc_H) -> std::uint32_t
{
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
                                                                                                                                     ((int32_t)dig_H6)) >>
                                                                                                                                    10) *
                                                                                                                                   (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
                                                                                                                                    ((int32_t)32768))) >>
                                                                                                                                  10) +
                                                                                                                                 ((int32_t)2097152)) *
                                                                                                                                    ((int32_t)dig_H2) +
                                                                                                                                8192) >>
                                                                                                                               14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)dig_H1)) >>
                              4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}

auto BME280::BME280_compensate_T_double(std::int32_t adc_T) -> double
{
    double var1, var2, T;
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) *
           ((double)dig_T3);
    t_fine = (int32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}
// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
auto BME280::BME280_compensate_P_double(std::int32_t adc_P) -> double
{
    double var1, var2, p;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}

auto BME280::BME280_compensate_H_double(int32_t adc_H) -> double
{
    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 *
                                                    var_H)) *
            (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * var_H * (1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    return var_H;
}
