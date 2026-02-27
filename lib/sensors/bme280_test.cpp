#include "include/bme280.h"
#include <stdio.h>
#include <iomanip>
#include <thread>
#include <chrono>

#define BME280_ADDR 0x76

int main() {
    
    BME280 strato_bme280(BME280_ADDR);
    auto initededed = strato_bme280.init();
    if (!initededed) {
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto values = strato_bme280.getTPHValues();

    double T = strato_bme280.BME280_compensate_T_double(values.adc_T);
    double P = strato_bme280.BME280_compensate_P_double(values.adc_P);
    double H = strato_bme280.BME280_compensate_H_double(values.adc_H);

    auto temp = strato_bme280.BME280_compensate_T_int32(values.adc_T)/100.;
    auto pres = strato_bme280.BME280_compensate_P_int64(values.adc_P)/256.;
    auto humi = strato_bme280.BME280_compensate_H_int32(values.adc_H)/1024.;

    std::cout << "T raw : 0x"
            << std::hex << std::setw(8) << std::setfill('0') << values.adc_T
            << std::dec << '\n';

    std::cout << "P raw : 0x"
            << std::hex << std::setw(8) << std::setfill('0') << values.adc_P
            << std::dec << '\n';

    std::cout << "H raw : 0x"
            << std::hex << std::setw(8) << std::setfill('0') << values.adc_H
            << std::dec << '\n';

    std::cout << "T converted (integer arith.) : " << T << " °C\n";
    std::cout << "T converted (integer arith.) : " << P << " Pa\n";
    std::cout << "T converted (integer arith.) : " << H << " % rF\n";
    std::cout << "T converted (double arith.) : " << temp << " °C\n";
    std::cout << "T converted (double arith.) : " << pres << " Pa\n";
    std::cout << "T converted (double arith.) : " << humi << " % rF\n";
    std::cout << std::flush;
}