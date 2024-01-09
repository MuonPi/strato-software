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

#include "main.h"
#include "muonpi_lmic.h"
#include "serialhandler.h"
#include <Arduino.h>

// TTN *****************************
// #define DEVICEID "eui-70B3D57ED0056189"
// #define ABP_DEVICEID "eui-70B3D57ED0056189"

// // The Network Session Key / DO NOT SHARE
// static const PROGMEM u1_t NWKSKEY[16] = {0x83, 0xF0, 0xD0, 0x76, 0x4A, 0x2D, 0x4F, 0x80, 0x01, 0x8D, 0xF5, 0x49, 0xB2, 0x80, 0xB6, 0x2E};

// // LoRaWAN AppSKey, application session key / DO NOT SHARE
// static const u1_t PROGMEM APPSKEY[16] = {0x1E, 0xBB, 0x6D, 0x2C, 0x2C, 0x41, 0xFE, 0x3A, 0x79, 0x1E, 0x18, 0x05, 0xBB, 0x8A, 0x4C, 0x8A};

// // LoRaWAN end-device address (DevAddr) / DO NOT SHARE
// static const u4_t DEVADDR = 0x260B6D09;

#define DEVICEID "eui-70B3D57ED0056189"
#define ABP_DEVICEID "eui-70B3D57ED0056189"

// The Network Session Key / DO NOT SHARE
static const PROGMEM u1_t NWKSKEY[16] = {0xCC, 0xB8, 0xF3, 0xD3, 0xFD, 0x39, 0x75, 0xAE, 0xE4, 0x84, 0x35, 0x90, 0xFE, 0x37, 0x1C, 0x88};

// LoRaWAN AppSKey, application session key / DO NOT SHARE
static const u1_t PROGMEM APPSKEY[16] = {0xA8, 0xDF, 0x3A, 0xC7, 0x51, 0xB2, 0xD1, 0x73, 0xAC, 0x58, 0x81, 0x91, 0xD2, 0x58, 0xCB, 0x4E};

// LoRaWAN end-device address (DevAddr) / DO NOT SHARE
static const u4_t DEVADDR = 0x260BC37E;

/********************************/

osjob_t workjob;

void(* resetFunc) (void) = 0;  //declare reset function at address 0

int sleepcycles = 1; // 130 X 8 seconds = ~17 mins sleep

// ============================================================================

MuonPiLMIC *muonpi_lmic;

SerialHandler *serial_handler;

unsigned count{0};

u4_t sequenceNo{1};
uint8_t data[259];

// ============================================================================

void setup()
{
#ifndef SERIAL_BAUD
    Serial.begin(9600, SERIAL_8N1);
#else
    Serial.begin(SERIAL_BAUD, SERIAL_8N1);
#endif
    // str.reserve(255);
    delay(10);
    serial_handler = new SerialHandler();

    // Create the LMIC object
    muonpi_lmic = new MuonPiLMIC();

    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are provided.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));

    // Setup LMIC
    muonpi_lmic->setup(DEVADDR, appskey, nwkskey, serial_handler);

    // process_work(&workjob);
    Serial.flush();
}

// ============================================================================

void loop()
{
    auto data_avail = serial_handler->read(data);
    // serial_handler->send(String(data[3]));
    if (data_avail>=4)
    {
        
        // serial_handler->send(String(data_avail));

        sequenceNo = data[0];
        sequenceNo = sequenceNo << 8;
        sequenceNo |= data[1];
        sequenceNo = sequenceNo << 8;
        sequenceNo |= data[2];
        sequenceNo = sequenceNo << 8;
        sequenceNo |= data[3];

        // serial_handler->send(String(sequenceNo));

        // auto test = str.c_str();
        // uint8_t* pld = (uint8_t*)malloc(str.length()-4);
        // for (size_t i = 4; i < str.length(); i++){
        //     pld[i-4] = static_cast<uint8_t>(str.charAt(i));
        // }
        // serial_handler->send(String(strlen(test)));
        // serial_handler->send(data,data_avail);
        // serial_handler->send(String("SequenceNo: ") + String(sequenceNo));
        
        // serial_handler->send(pld, str.length()-4);
        data_avail = data_avail - 4;
        for (size_t i = 0; i < data_avail; i++){
            data[i] = data[i+4];
        }
        // serial_handler->send(data, data_avail);
        // serial_handler->send(str);
		// bis hier alles ok

        // muonpi_lmic->sendLoraPayload(1u, sequenceNo, str);
        muonpi_lmic->sendLoraPayload(1u, sequenceNo, data, data_avail);
    }
    os_runloop_once();
}

// ============================================================================