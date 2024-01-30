// /*******************************************************************************
//  * AmbaSat-1
//  * Filename: AmbaSatLMIC.cpp
//  *
//  * This library is designed for use with AmbaSat-1 and is a wrapper for
//  * IBM LMIC functionality
//  *
//  * Copyright (c) 2022 AmbaSat Ltd
//  * https://ambasat.com
//  *
//  * licensed under Creative Commons Attribution-ShareAlike 3.0
//  *******************************************************************************/

// /*******************************************************************************
//  *                Leds                GPIO
//  *                ----                ----
//  *                LED   <――――――――――>  13  (LED_BUILTIN) (SCK) Active-high,
//  *                                        Useless, shared with SCK.
//  *
//  *                I2C [display]       GPIO
//  *                ---                 ----
//  *                SDA   <――――――――――>   2  (SDA)
//  *                SCL   <――――――――――>   3  (SCL)
//  *
//  *                SPI/LoRa module     GPIO
//  *                ----                ----
//  *                MOSI  <――――――――――>  11  (MOSI)
//  *                MISO  <――――――――――>  12  (MISO)
//  *                SCK   <――――――――――>  13  (SCK)
//  *                NSS   <――――――――――>  10  (SS)
//  *                RST   <――――――――――>   7
//  *                DIO0  <――――――――――>   8
//  *                DIO1  <――――――――――>   9
//  *                DIO2                 -  Not needed for LoRa.
//  *******************************************************************************/

#include "strato_lmic.h"
#include "lmic.h"
#include <iostream>
#include "hal/hal.h"
// #include "hal/raspi-spi.h"   // dont include, dont ask why
#include "lmic/oslmic.h"
#include "lmic/lmic.h"
#include <gpiod.h>

bool joined = false;
bool sleeping = false;

// static osjob_t sendjob;
static osjob_t sendjob;

uint32_t uplinkSequenceNo = 0; // aka FCnt

uint8_t* StratoLMIC::data;
uint8_t StratoLMIC::data_size{};

// // arduino lmic pin mapping
// const lmic_pinmap lmic_pins = {
//     .nss = 10,
//     .rxtx = LMIC_UNUSED_PIN,
//     .rst = 9 // 7,
//     .dio = {/*dio0*/ 6, /*dio1*/ 7, /*dio2*/ 8} // (8, 9, LMIC_UNUSED_PIN)
// //     .rxtx_rx_active = 0,
// //     .rssi_cal = 10,
// //     .spi_freq = 1000000 /* 1 MHz */
// };


// Strato-Board		//NKRG
#define RF_CS_PIN  0x24 // RPI_V2_GPIO_P1_24 // Slave Select on GPIO25 so P1 connector pin #24
#define RF_IRQ_PIN 0x38 // RPI_V2_GPIO_P1_38 // DIO0 on GPIO20 so P1 connector pin #38
#define RF_RST_PIN 0x40 // RPI_V2_GPIO_P1_40 // Reset on GPIO21 so P1 connector pin #40

// Pin mapping		//NKRG
const lmic_pinmap lmic_pins = { 
    .nss  = RF_CS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = RF_RST_PIN,
    .dio  = {RF_IRQ_PIN, LMIC_UNUSED_PIN, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 10,
    .spi_freq = 1000000 /* 1 MHz */
};

ostime_t os_getTime();  // why does this work

// ======================================================================================

void printEvent(ev_t ev){
}

// =========================================================================================================================================
// onEvent
// =========================================================================================================================================

void StratoLMIC::onEvent(void *pUserData, ev_t ev)
{
    switch (ev)
    {
    case EV_RXSTART:
        // Do not print anything for this event or it will mess up timing.
        break;

    case EV_TXSTART:
        // std::cout << std::to_string(os_getTime()) << ": EV_TXSTART" << std::endl;
        break;
    case EV_JOIN_TXCOMPLETE:
        std::cout << "EV_JOIN_TXCOMPLETE" << std::endl;
    case EV_TXCANCELED:
        std::cout << "EV_TXCANCELLED" << std::endl;
        break;
    case EV_SCAN_TIMEOUT:
        std::cout << "EV_SCAN_TIMEOUT" << std::endl;
        break;
    case EV_BEACON_FOUND:
        std::cout << "EV_BEACON_FOUND" << std::endl;
        break;
    case EV_BEACON_MISSED:
        std::cout << "EV_BEACON_MISSED" << std::endl;
        break;
    case EV_BEACON_TRACKED:
        std::cout << "EV_BEACON_TRACKED" << std::endl;
        break;
    case EV_JOINING:
        std::cout << "EV_JOINING" << std::endl;
        break;
    case EV_JOINED:
        std::cout << "EV_JOINED" << std::endl;
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_RFU1:
    ||     m_serial_handler->send(F("EV_RFU1"));
    ||     break;
    */
    case EV_JOIN_FAILED:
        std::cout << "EV_JOIN_FAILED" << std::endl;
        break;
    case EV_REJOIN_FAILED:
        std::cout << "EV_REJOIN_FAILED" << std::endl;
        break;
    case EV_TXCOMPLETE:
        std::cout << "EV_TXCOMPLETE" << std::endl;
        // if (LMIC.txrxFlags & TXRX_ACK)
            std::cout << "Received ack" << std::endl;
        // if (LMIC.dataLen)
        {
            std::cout << "Received ";
            // std::cout << static_cast<int>(LMIC.dataLen);
            std::cout << " bytes of payload" << std::endl;
        }
        // Schedule next transmission
        // will be called by main loop
        break;
    case EV_LOST_TSYNC:
        std::cout << "EV_LOST_TSYNC" << std::endl;
        break;
    case EV_RESET:
        std::cout << "EV_RESET" << std::endl;
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        std::cout << "EV_RXCOMPLETE" << std::endl;
        break;
    case EV_LINK_DEAD:
        std::cout << "EV_LINK_DEAD" << std::endl;
        break;
    case EV_LINK_ALIVE:
        std::cout << "EV_LINK_ALIVE" << std::endl;
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    m_serial_handler->send(F("EV_SCAN_FOUND"));
    ||    break;
    */
    default:
        std::cout << "Unknown event: " << std::endl;
        // m_serial_handler->send((unsigned)ev);
        break;
    }
}

// from here uncomment
// ======================================================================================

bool StratoLMIC::setup(devaddr_t devaddr, unsigned char *appskey, unsigned char *nwkskey)
{
    // os_init(); // LMIC init

    std::cout << "Starting" << std::endl;
    // LMIC_reset(); // Reset the MAC state. Session and pending data transfers will be discarded.

    // network ID 0x01 = Expiremental
    // network ID 0x13 = The Things Network
    // LMIC_setSession(0x13, devaddr, nwkskey, appskey);

    // LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
    // LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK), BAND_MILLI);   // g2-band

    // Disable link check validation
    // LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    // LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    // LMIC_setDrTxpow(static_cast<dr_t>(DR_SF10), static_cast<s1_t>(20)); // spreading factor 10

    // LMIC_setAdrMode(true); // adr on

    uint32_t clockError = (LMIC_CLOCK_ERROR_PPM / 100) * (MAX_CLOCK_ERROR / 100) / 100;
    // LMIC_setClockError(clockError);

    // LMIC_registerEventCb(&onEvent, nullptr);
    return true;
}

// // ======================================================================================

// void StratoLMIC::do_send(osjob_t *workjob)
// {
//     // Check if there is not a current TX/RX job running
//     if (LMIC.opmode & OP_TXRXPEND)
//     {
//         std::cout << "OP_TXRXPEND, not sending" << std::endl;
//         return;
//     }
//     else
//     {
//         // m_serial_handler->send("Answer: " + String(data.c_str()));
//         // auto buf = reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str()));
//         // m_serial_handler->send(data);
//         // LMIC_setTxData2(1, reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str())), data.length(), 0);
//         LMIC_setTxData2(1, data, data_size, 0);
//     }
//     // Next TX is scheduled after TX_COMPLETE event.
// }


// void StratoLMIC::sendLoraPayload(u1_t port, u4_t sequenceNo, uint8_t* message, uint8_t n)
// {
//     data = message;
//     data_size = n;
//     LMIC.seqnoUp = sequenceNo;

//     os_setCallback(&sendjob, do_send);
// }

