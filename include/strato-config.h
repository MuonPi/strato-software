
#ifndef _STRATO_CONFIG_H_
#define _STRATO_CONFIG_H_



#define SENSORS_INTERVAL 1 //s
#define SENSORS_TIMEOUT 3 //s
#define SENSORS_RESTART 2 //s
#define LORAWAN_INTERVAL 60 //s
#define LORAWAN_TIMEOUT 30 //s
#define LORAWAN_RESTART 5 //s
#define WATCHDOG_INTERVAL 500 //ms
#define LOGFILE_INTERVAL 30 //min

#define MUONPI_USED
#define ADS1115_ADDR 0x4A
#define BATTERY_VOLTAGE_USED
// #define SOLAR_VOLTAGE_USED
// #define GUVAS12SD_USED
// #define QMC5883_ADDR 0x0D
// #define VEML6075_ADDR 0x10
// #define LTR390UV01_ADDR 0x53
#define AS7331_ADDR 0x74
#define AS7343_ADDR 0x39
#define BME280_ADDR 0x76
#define BME280_2_ADDR 0x77
// #define SHT31_ADDR 0x44
// #define MPU6050_ADDR 0x68
// #define SEN0321_ADDR 0x73
// #define OZONE3CLICK_LMP_ADDR 0x48
// #define OZONE3CLICK_ADC_ADDR 0x4D

#define VOLTAGE_DIVIDER_A0_R1 20
#define VOLTAGE_DIVIDER_A0_R2 51
// #define VOLTAGE_DIVIDER_A1_R1 51
// #define VOLTAGE_DIVIDER_A1_R2 51
// #define VOLTAGE_DIVIDER_A2_R1
// #define VOLTAGE_DIVIDER_A2_R2
// #define VOLTAGE_DIVIDER_A3_R1
// #define VOLTAGE_DIVIDER_A3_R2


// Strato-Mainboard
#define RF_CS_PIN 8
#define RF_IRQ_PIN 20
#define RF_RST_PIN 21


// #define DISABLE_BEACONS 1
// #define DISABLE_JOIN 1
// #define DISABLE_PING 1
// #define LMIC_DEBUG_LEVEL 1
// #define CFG_eu868 1
// #define CFG_sx1276_radio 1
#define LMIC_CLOCK_ERROR_PPM 30000



#define DEVICEID "XXXXXXXXXXXXXXXX"     // skymi
#define ABP_DEVICEID "XXXXXXXXXXXXXXXX"

// The Network Session Key / DO NOT SHARE
inline constexpr uint8_t NWKSKEY[16] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
inline constexpr uint8_t APPSKEY[16] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
inline constexpr uint32_t DEVADDR = 0xXXXXXXXX;



#endif // _STRATO_CONFIG_H_
