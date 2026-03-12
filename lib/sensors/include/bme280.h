#ifndef _BME280_H_
#define _BME280_H_

#include "i2cdevice.h"
#include <cstdint>

/* BME280  */
// struct to store temperature, pressure and humidity data in different ways
struct TPH {
    std::int32_t adc_T;
    std::int32_t adc_P;
    std::int32_t adc_H;
    double T, P, H;
};
class BME280 : public i2cDevice { // t_max = 112.8 ms for all three measurements at max oversampling
public:
    BME280()
        : i2cDevice(0x76)
    {
        // init();
    }
    BME280(const char* busAddress, std::int8_t slaveAddress)
        : i2cDevice(busAddress, slaveAddress)
    {
        // init();
    }
    BME280(std::int8_t slaveAddress)
        : i2cDevice(slaveAddress)
    {
        // init();
    }

    bool init();
    auto getTPHValues() -> TPH;
    auto BME280_compensate_T_int32(std::int32_t adc_T) -> std::int32_t;
    auto BME280_compensate_P_int64(std::int32_t adc_P) -> std::uint32_t;
    auto BME280_compensate_H_int32(std::int32_t adc_H) -> std::uint32_t;
    auto BME280_compensate_T_double(std::int32_t adc_T) -> double;
    auto BME280_compensate_P_double(std::int32_t adc_P) -> double;
    auto BME280_compensate_H_double(int32_t adc_H) -> double;

private:
    void readCalibParameters();
    TPH readTPCU();

    std::int32_t t_fine{};
    unsigned int fLastConvTime;
    bool fCalibrationValid;
    std::uint16_t fCalibParameters[18];
    
    std::int32_t dig_T1;
    std::int32_t dig_T2;
    std::int32_t dig_T3;
    std::int32_t dig_P1;
    std::int32_t dig_P2;
    std::int32_t dig_P3;
    std::int32_t dig_P4;
    std::int32_t dig_P5;
    std::int32_t dig_P6;
    std::int32_t dig_P7;
    std::int32_t dig_P8;
    std::int32_t dig_P9;
    std::int32_t dig_H1;
    std::int32_t dig_H2;
    std::int32_t dig_H3;
    std::int32_t dig_H4;
    std::int32_t dig_H5;
    std::int32_t dig_H6;
};

#endif // !_BME280_H_
