/*******************************************************************************
 * AmbaSat-1
 * Filename: main.cpp
 * AmbaSat-1 Flight Code for Sensor 01 - SHT31: Temperature & Humidity
 * 16th January 2022
 * Authors: Martin Platt, James Vonteb
 *
 * Copyright (c) 2022 AmbaSat Ltd
 * https://ambasat.com
 *
 * To use this code, set NWKSKEY, APPSKEY & DEVADDR values as per your Dashboard
 * See the HowTo: https://ambasat.com/howto/kit-2/#/../unique-ids
 *
 * For ISM band configuration: See lmic/config.h eg. #define CFG_us915 1
 * licensed under Creative Commons Attribution-ShareAlike 3.0
 * ******************************************************************************/

#include <stdio.h>
#include <iostream>
#include "main.h"
#include "strato_lmic.h"
#include "lmic.h"
// #include "lib_lmic.so"

u1_t hallo;

// TTN *****************************
// #define DEVICEID "eui-70B3D57ED0056189"
// #define ABP_DEVICEID "eui-70B3D57ED0056189"

// // The Network Session Key / DO NOT SHARE
// static const PROGMEM u1_t NWKSKEY[16] = {0x83, 0xF0, 0xD0, 0x76, 0x4A, 0x2D, 0x4F, 0x80, 0x01, 0x8D, 0xF5, 0x49, 0xB2, 0x80, 0xB6, 0x2E};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t PROGMEM APPSKEY[16] = {0x1E, 0xBB, 0x6D, 0x2C, 0x2C, 0x41, 0xFE, 0x3A, 0x79, 0x1E, 0x18, 0x05, 0xBB, 0x8A, 0x4C, 0x8A};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260B6D09;

// from here uncomment
// #define DEVICEID "eui-70B3D57ED0056189"
// #define ABP_DEVICEID "eui-70B3D57ED0056189"

// // The Network Session Key / DO NOT SHARE
// static const PROGMEM u1_t NWKSKEY[16] = {0xCC, 0xB8, 0xF3, 0xD3, 0xFD, 0x39, 0x75, 0xAE, 0xE4, 0x84, 0x35, 0x90, 0xFE, 0x37, 0x1C, 0x88};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t PROGMEM APPSKEY[16] = {0xA8, 0xDF, 0x3A, 0xC7, 0x51, 0xB2, 0xD1, 0x73, 0xAC, 0x58, 0x81, 0x91, 0xD2, 0x58, 0xCB, 0x4E};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260BC37E;


// /********************************/

// // osjob_t workjob;

// // void(* resetFunc) (void) = 0;  //declare reset function at address 0

// // int sleepcycles = 1; // 130 X 8 seconds = ~17 mins sleep

// // ============================================================================

// // StratoLMIC *strato_lmic;

// // unsigned count{0};

// uint8_t message[255];


// // u4_t sequenceNo{1};
// // uint8_t data[259];

// // ============================================================================

int main()
{
//     for (int i = 0; i < sizeof(message); i++)
//     {
//         message[i] = 0;
//     }
//     std::cout << sizeof(message) << std::endl;
//     // Create the LMIC object
//     // strato_lmic = new StratoLMIC();

//     // Set static session parameters. Instead of dynamically establishing a session
//     // by joining the network, precomputed session parameters are provided.
//     uint8_t appskey[sizeof(APPSKEY)];
//     uint8_t nwkskey[sizeof(NWKSKEY)];
//     memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
//     memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));

//     // Setup LMIC
//     // strato_lmic->setup(DEVADDR, appskey, nwkskey);

//     // process_work(&workjob);

//     // Sending message
//     // strato_lmic->sendLoraPayload(1u, sequenceNo, data, len);
//     // os_runloop_once();
    std::cout << "end of main" << std::endl;
}
