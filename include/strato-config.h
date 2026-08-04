
#ifndef _STRATO_CONFIG_H_
#define _STRATO_CONFIG_H_



#define SENSORS_INTERVAL 1 //s
#define SENSORS_TIMEOUT 3 //s
#define SENSORS_RESTART 2 //s
#define LORAWAN_INTERVAL 60 //s
#define LORAWAN_TIMEOUT 30 //s
#define LORAWAN_RESTART 5 //s
#define OOK_INTERVAL LORAWAN_INTERVAL //s
#define OOK_TIMEOUT 5 //s
#define OOK_RESTART 2 //s
#define WATCHDOG_INTERVAL 500 //ms
#define LOGFILE_INTERVAL 30 //min

#define OOK_USED
#define OOK_GPIO_PIN 7
#define OOK_HALF_BIT_US 500
#define OOK_REPEAT_GAP_MS 20
#define OOK_REPEATS 3
#define OOK_PAYLOAD_FORMAT_CAYENNE_LPP 0
#define OOK_PAYLOAD_FORMAT_TEXT 1
#define OOK_PAYLOAD_FORMAT OOK_PAYLOAD_FORMAT_TEXT

#define MUONPI_USED
#define ADS1115_ADDR 0x4A
// #define QMC5883_ADDR 0x0D
// #define VEML6075_ADDR 0x10
#define LTR390UV01_ADDR 0x53
// #define AS7331_ADDR 0x74
// #define AS7343_ADDR 0x39
#define BME280_ADDR 0x76
// #define SHT31_ADDR 0x44
// #define MPU6050_ADDR 0x68
// #define SEN0321_ADDR 0x73
// #define OZONE3CLICK_LMP_ADDR 0x48
// #define OZONE3CLICK_ADC_ADDR 0x4D

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



#define DEVICEID "eui-70b3d57ed0052abe"     // Arduino-Test-0
#define ABP_DEVICEID "eui-70b3d57ed0052abe"

// The Network Session Key / DO NOT SHARE
inline constexpr uint8_t NWKSKEY[16] = {0xCC, 0xB8, 0xF3, 0xD3, 0xFD, 0x39, 0x75, 0xAE, 0xE4, 0x84, 0x35, 0x90, 0xFE, 0x37, 0x1C, 0x88};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
inline constexpr uint8_t APPSKEY[16] = {0xA8, 0xDF, 0x3A, 0xC7, 0x51, 0xB2, 0xD1, 0x73, 0xAC, 0x58, 0x81, 0x91, 0xD2, 0x58, 0xCB, 0x4E};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
inline constexpr uint32_t DEVADDR = 0x260BC37E;




// #define DEVICEID "eui-70B3D57ED006549C"      // strato-abp
// #define ABP_DEVICEID "eui-70B3D57ED006549C"

// // The Network Session Key / DO NOT SHARE
// static const u1_t NWKSKEY[16] = {0x3B, 0x78, 0xFB, 0x1B, 0x52, 0xFF, 0xDD, 0xC0, 0xA1, 0x4B, 0xB3, 0x50, 0x12, 0x05, 0x6A, 0x5B};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t APPSKEY[16] = {0x3F, 0x39, 0xB5, 0xFF, 0x34, 0x23, 0x44, 0xC4, 0x2C, 0x61, 0x25, 0x52, 0xFF, 0x4F, 0x3F, 0xD1};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260B51D0;



#endif // _STRATO_CONFIG_H_
