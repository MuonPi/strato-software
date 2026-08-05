
#ifndef _STRATO_CONFIG_H_
#define _STRATO_CONFIG_H_



#define SENSORS_INTERVAL 1 //s
#define SENSORS_TIMEOUT 3 //s
#define SENSORS_RESTART 2 //s
#define LORAWAN_INTERVAL 60 //s
#define LORAWAN_TIMEOUT 30 //s
#define LORAWAN_RESTART 5 //s
#define OOK_INTERVAL 5 //s
#define OOK_TIMEOUT 5 //s
#define OOK_RESTART 2 //s
#define WATCHDOG_INTERVAL 500 //ms
#define LOGFILE_INTERVAL 30 //min

// #define OOK_USED
#define OOK_GPIO_PIN 7
#define OOK_HALF_BIT_US 500
#define OOK_REPEAT_GAP_MS 20
#define OOK_REPEATS 3
#define OOK_PAYLOAD_FORMAT_CAYENNE_LPP 0
#define OOK_PAYLOAD_FORMAT_TEXT 1
#define OOK_PAYLOAD_FORMAT OOK_PAYLOAD_FORMAT_TEXT

#define MUONPI_USED
#define ADS1115_ADDR 0x4A
#define QMC5883_ADDR 0x0D
// #define VEML6075_ADDR 0x10
#define LTR390UV01_ADDR 0x53
#define AS7331_ADDR 0x74
#define AS7343_ADDR 0x39
#define BME280_ADDR 0x76
#define SHT31_ADDR 0x44
#define MPU6050_ADDR 0x68
// #define SEN0321_ADDR 0x73
#define OZONE3CLICK_LMP_ADDR 0x48
#define OZONE3CLICK_ADC_ADDR 0x4D

#define VOLTAGE_DIVIDER_A0_R1 20
#define VOLTAGE_DIVIDER_A0_R2 51
#define VOLTAGE_DIVIDER_A1_R1 51
#define VOLTAGE_DIVIDER_A1_R2 51
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



#define DEVICEID "70B3D57ED0078A25"     // skymi strato
#define ABP_DEVICEID "70B3D57ED0078A25"

// The Network Session Key / DO NOT SHARE
inline constexpr uint8_t NWKSKEY[16] = {0x70, 0x22, 0xB6, 0x4D, 0xD2, 0xA9, 0x9F, 0xC4, 0xAE, 0x34, 0xA0, 0x1D, 0xAD, 0xAD, 0x6F, 0xE1};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
inline constexpr uint8_t APPSKEY[16] = {0x3A, 0xCC, 0xA3, 0xC0, 0xE4, 0x0A, 0x46, 0x4A, 0xBF, 0x12, 0x16, 0x6A, 0x44, 0xF4, 0xCF, 0x0A};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
inline constexpr uint32_t DEVADDR = 0x260B88EF;



#endif // _STRATO_CONFIG_H_
