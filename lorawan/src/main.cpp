/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses ABP (Activation-by-personalisation), where a DevAddr and
 * Session keys are preconfigured (unlike OTAA, where a DevEUI and
 * application key is configured, while the DevAddr and session keys are
 * assigned/generated in the over-the-air-activation procedure).
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!
 *
 * To use this sketch, first register your application and device with
 * the things network, to set or generate a DevAddr, NwkSKey and
 * AppSKey. Each device should have their own unique values for these
 * fields.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

 // References:
 // [feather] adafruit-feather-m0-radio-with-lora-module.pdf

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "strato_lmic.h"
#include "lmic.h"
#include "hal/hal.h"
#include "raspi/raspi.h"

#include "spidevice.h"
#include <iomanip>



void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }


// #define DISABLE_BEACONS 1
// #define DISABLE_JOIN 1
// #define DISABLE_PING 1

// #define LMIC_ENABLE_user_events 1

#define DEVICEID "eui-70b3d57ed0052abe"     // Arduino-Test-0
#define ABP_DEVICEID "eui-70b3d57ed0052abe"

// The Network Session Key / DO NOT SHARE
static const u1_t NWKSKEY[16] = {0xCC, 0xB8, 0xF3, 0xD3, 0xFD, 0x39, 0x75, 0xAE, 0xE4, 0x84, 0x35, 0x90, 0xFE, 0x37, 0x1C, 0x88};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
static const u1_t APPSKEY[16] = {0xA8, 0xDF, 0x3A, 0xC7, 0x51, 0xB2, 0xD1, 0x73, 0xAC, 0x58, 0x81, 0x91, 0xD2, 0x58, 0xCB, 0x4E};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
static const u4_t DEVADDR = 0x260BC37E;




// #define DEVICEID "eui-70B3D57ED006549C"      // srato-abp
// #define ABP_DEVICEID "eui-70B3D57ED006549C"

// // The Network Session Key / DO NOT SHARE
// static const u1_t NWKSKEY[16] = {0x3B, 0x78, 0xFB, 0x1B, 0x52, 0xFF, 0xDD, 0xC0, 0xA1, 0x4B, 0xB3, 0x50, 0x12, 0x05, 0x6A, 0x5B};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t APPSKEY[16] = {0x3F, 0x39, 0xB5, 0xFF, 0x34, 0x23, 0x44, 0xC4, 0x2C, 0x61, 0x25, 0x52, 0xFF, 0x4F, 0x3F, 0xD1};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260B51D0;


uint32_t uplinkSequenceNo;
uint8_t payload[256];
uint8_t len;
const unsigned TX_INTERVAL = 60;
osjob_t workjob;




// Strato-Mainboard		//NKRG
#define RF_CS_PIN 8
#define RF_IRQ_PIN 20
#define RF_RST_PIN 21

// Pin mapping
const lmic_pinmap lmic_pins = 
{
    .nss = RF_CS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RF_RST_PIN,
    .dio = {RF_IRQ_PIN, RF_IRQ_PIN, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 10,
    .spi_freq = 1000000 /* 1 MHz */
};


int main()
{


    // SPI::spiDevice spi_device{};
    // spi_device.init();
    // for (std::size_t i{0}; i < 0x64; i++)
    // {
    //     auto ch = spi_device.read(i, 1)[0];
    //     std::cout << i << ": " << std::hex << static_cast<unsigned>(ch) << std::endl;
    // }




    uint8_t nwkskey[sizeof(NWKSKEY)];
    uint8_t appskey[sizeof(APPSKEY)];

    for (int i = 0; i < 16; i++)
    {
        nwkskey[i] = NWKSKEY[i];
        appskey[i] = APPSKEY[i];
    }

    setup(DEVADDR, lmic_pins, appskey, nwkskey);
    // os_runloop_once();

    std::cout << "finished setup function" << std::endl;

    uplinkSequenceNo = SeqNoFile();
    std::cout << static_cast<int>(uplinkSequenceNo) << std::endl;

    len = PayloadFile(payload);
    std::cout << static_cast<int>(len) << std::endl;
    for (size_t i = 0; i < len; i++)
    {
        std::cout << static_cast<int>(payload[i]) << " ";
    }
    std::cout << std::endl;


    // for (std::size_t i{0}; i < 0x64; i++)
    // {
    //     auto ch = spi_device.read(i, 1)[0];
    //     std::cout << i << ": " << std::hex << static_cast<unsigned>(ch) << std::endl;
    // }

    sendLoraPayload(1u, uplinkSequenceNo, payload, len);


    while(1)
    {
        os_runloop_once();
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    
    // std::cout << "EV_TXCOMPLETE manually" << std::endl;


    // for (std::size_t i{0}; i < 0x64; i++)
    // {
    //     auto ch = spi_device.read(i, 1)[0];
    //     std::cout << i << ": " << std::hex << static_cast<unsigned>(ch) << std::endl;
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(60000));
    
    return 0;
}








// #include <iostream>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/spi/spidev.h>
// #include <cstring>

// const char *SPI_DEVICE = "/dev/spidev0.0";
// uint8_t SPI_MODE = SPI_MODE_0;
// uint8_t BITS_PER_WORD = 8;
// uint32_t SPI_SPEED = 1000000; // 1 MHz

// // Register-Defines
// #define REG_FIFO          0x00
// #define REG_OP_MODE       0x01
// #define REG_IRQ_FLAGS     0x12
// #define REG_IRQ_FLAGS_MASK 0x11
// #define REG_DIO_MAPPING1  0x40

// int spi_fd = -1;

// // SPI open / close
// bool spi_init() {
//     spi_fd = open(SPI_DEVICE, O_RDWR);
//     if (spi_fd < 0) return false;

//     if (ioctl(spi_fd, SPI_IOC_WR_MODE, &SPI_MODE) < 0) return false;
//     if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &BITS_PER_WORD) < 0) return false;
//     if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) < 0) return false;

//     return true;
// }

// void spi_close() {
//     if (spi_fd >= 0) close(spi_fd);
// }

// // SPI Write Register
// void writeReg(uint8_t reg, uint8_t val) {
//     uint8_t buf[2] = { reg | 0x80, val }; // MSB=1 für Write
//     write(spi_fd, buf, 2);
// }

// // SPI Read Register
// uint8_t readReg(uint8_t reg) {
//     uint8_t buf[2] = { reg & 0x7F, 0x00 };
//     write(spi_fd, buf, 2);
//     uint8_t rx[2];
//     read(spi_fd, rx, 2);
//     return rx[1];
// }

// // Minimaler TX-Puls
// void rfm95_tx_test() {
//     if (!spi_init()) {
//         std::cerr << "SPI init failed\n";
//         return;
//     }

//     std::cout << "=== RFM95 Minimal TX-Test ===\n";

//     // 1. Standby + LoRa
//     writeReg(REG_OP_MODE, 0x81);  // LoRa, Standby

//     // 2. DIO0 auf TxDone / RxDone
//     writeReg(REG_DIO_MAPPING1, 0x00);

//     // 3. IRQ unmask
//     writeReg(REG_IRQ_FLAGS_MASK, 0x00);

//     // 4. IRQ Flags löschen
//     writeReg(REG_IRQ_FLAGS, 0xFF);

//     // 5. Dummy Payload in FIFO
//     writeReg(REG_FIFO, 0xAA);  // 1 Byte Dummy

//     // 6. TX starten
//     writeReg(REG_OP_MODE, 0x83); // Mode TX

//     std::cout << "TX gestartet. Prüfe DIO0 am Oszi!\n";

//     // 7. Optional: Warte kurz und lese IRQ
//     usleep(200000); // 200 ms warten
//     uint8_t irq = readReg(REG_IRQ_FLAGS);
//     std::cout << "IRQ Flags nach TX: 0x" << std::hex << (int)irq << std::dec << "\n";

//     spi_close();
// }


// int main() {
//     rfm95_tx_test();
//     return 0;
// }
