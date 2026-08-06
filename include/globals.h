#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <atomic>
#include <cstdint>


struct Globals
{
    std::atomic<double> XOR {0};
    std::atomic<double> XOR_mean {0};
    std::atomic<uint16_t> XOR_count {0};

    std::atomic<double> AND {0};
    std::atomic<double> AND_mean {0};
    std::atomic<uint16_t> AND_count {0};

    std::atomic<double> position[3] {0};
    std::atomic<uint64_t> position_update_count {0};
    std::atomic<uint8_t> gps_fix {0};

    std::atomic<double> battery_voltage {0};
    std::atomic<double> battery_voltage_mean {0};
    std::atomic<uint16_t> battery_voltage_count {0};

    std::atomic<double> solar_voltage {0};
    std::atomic<double> solar_voltage_mean {0};
    std::atomic<uint16_t> solar_voltage_count {0};

    // std::atomic<double> magnet {0};
    // std::atomic<double> magnet_mean {0};
    // std::atomic<double> magnetXYZ[3] {0};
    // std::atomic<uint16_t> magnet_count {0};

    // std::atomic<double> uv[4] {0};
    // std::atomic<double> uv_mean {0};
    // std::atomic<uint16_t> uv_count {0};

    // std::atomic<double> ltr390_uv {0};
    // std::atomic<double> ltr390_uv_mean {0};
    // std::atomic<uint16_t> ltr390_uv_count {0};

    // std::atomic<double> as7331_uv[3] {0};
    // std::atomic<double> as7331_uv_mean[3] {0};
    // std::atomic<uint16_t> as7331_uv_count {0};

    // std::atomic<double> as7343_spectrum[18] {0};
    // std::atomic<double> as7343_spectrum_mean[18] {0};
    // std::atomic<uint16_t> as7343_spectrum_count {0};

    std::atomic<double> temperature {0};
    std::atomic<double> temperature_mean {0};
    std::atomic<uint16_t> temperature_count {0};

    std::atomic<double> pressure {0};
    std::atomic<double> pressure_mean {0};
    std::atomic<uint16_t> pressure_count {0};

    // std::atomic<double> humidity {0};
    // std::atomic<double> humidity_mean {0};
    // std::atomic<uint16_t> humidity_count {0};

    // std::atomic<double> acceleration[3] {0};
    // std::atomic<double> acceleration_mean[3] {0};
    // std::atomic<double> gyroscope[3] {0};
    // std::atomic<double> gyroscope_mean[3] {0};
    // std::atomic<double> mpu6050_temperature {0};
    // std::atomic<double> mpu6050_temperature_mean {0};
    // std::atomic<uint16_t> mpu6050_count {0};

    // std::atomic<double> uv_guvas12sd {0};
    // std::atomic<double> uv_guvas12sd_mean {0};
    // std::atomic<uint16_t> uv_guvas12sd_count {0};

    // std::atomic<double> ozone {0};
    // std::atomic<double> ozone_mean {0};
    // std::atomic<uint16_t> ozone_count {0};
};

#endif // _GLOBALS_H_
