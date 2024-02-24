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
#include <strato_lmic.h>
#include <lmic.h>
#include <hal/hal.h>
#include <raspi/raspi.h>



void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }


#define DISABLE_BEACON 1

// #define DEVICEID "eui-70b3d57ed0052abe"     // Arduino-Test-0
// #define ABP_DEVICEID "eui-70b3d57ed0052abe"

// // The Network Session Key / DO NOT SHARE
// static const u1_t NWKSKEY[16] = {0xCC, 0xB8, 0xF3, 0xD3, 0xFD, 0x39, 0x75, 0xAE, 0xE4, 0x84, 0x35, 0x90, 0xFE, 0x37, 0x1C, 0x88};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t APPSKEY[16] = {0xA8, 0xDF, 0x3A, 0xC7, 0x51, 0xB2, 0xD1, 0x73, 0xAC, 0x58, 0x81, 0x91, 0xD2, 0x58, 0xCB, 0x4E};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260BC37E;




#define DEVICEID "eui-70B3D57ED006549C"
#define ABP_DEVICEID "eui-70B3D57ED006549C"

// The Network Session Key / DO NOT SHARE
static const u1_t NWKSKEY[16] = {0x3B, 0x78, 0xFB, 0x1B, 0x52, 0xFF, 0xDD, 0xC0, 0xA1, 0x4B, 0xB3, 0x50, 0x12, 0x05, 0x6A, 0x5B};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
static const u1_t APPSKEY[16] = {0x3F, 0x39, 0xB5, 0xFF, 0x34, 0x23, 0x44, 0xC4, 0x2C, 0x61, 0x25, 0x52, 0xFF, 0x4F, 0x3F, 0xD1};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
static const u4_t DEVADDR = 0x260B51D0;


uint8_t mydata[] = "Hello world!";
const unsigned TX_INTERVAL = 60;
uint32_t uplinkSequenceNo = 2;




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
    .dio = {RF_IRQ_PIN, LMIC_UNUSED_PIN, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 10,
    .spi_freq = 1000000 /* 1 MHz */
};


int main()
{
    uint8_t nwkskey[sizeof(NWKSKEY)];
    uint8_t appskey[sizeof(APPSKEY)];

    uplinkSequenceNo = SeqNoFile();

    for (int i = 0; i < 16; i++)
    {
        nwkskey[i] = NWKSKEY[i];
        appskey[i] = APPSKEY[i];
    }

    init_gpio();
    setup(DEVADDR, lmic_pins, appskey, nwkskey);

    std::cout << "finished setup function" << std::endl;

    uint8_t len = sizeof(mydata);
    sendLoraPayload(1u, uplinkSequenceNo, mydata, len);
    uint8_t val{};
    // do{
    //     val = digitalRead(lmic_pins.dio[0]);
    // } while(val == 0);
    // std::cout << "digitalRead of dio0 = " << static_cast<unsigned>(val) << std::endl;
    // std::cout << "EV_TXCOMPLETE" << std::endl;
}