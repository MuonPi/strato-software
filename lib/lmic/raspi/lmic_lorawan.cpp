
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <libgen.h>
#include <algorithm>
#include <cctype>
#include <filesystem>

// #include "hal/hal.h"
// #include "lmic.h"
#include "lmic_lorawan.h"



void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;

bool lorawan_txcomplete = false;


void printEvent(ev_t ev)
{
}


void onEvent(void *pUserData, ev_t ev)
{
    // std::cout << "onEvent start" << std::endl;
    // std::cout << std::dec << static_cast<int>(ev) << std::endl;
    switch (ev)
    {
    case EV_RXSTART:
        // Do not print anything for this event or it will mess up timing.
        break;

    case EV_TXSTART:
        std::cout << std::to_string(os_getTime()) << ": EV_TXSTART" << std::endl;
        // dump_rfm96_registers();
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
        if (LMIC.txrxFlags & TXRX_ACK)
        {
            std::cout << "LMIC.txrxFlags: " << static_cast<int>(LMIC.txrxFlags) << "   TXRX_ACK: " << TXRX_ACK << "   LMIC.txrxFlags & TXRX_ACK: " << static_cast<int>(LMIC.txrxFlags & TXRX_ACK) << std::endl;
            std::cout << "Received ack" << std::endl;
        }
        if (LMIC.dataLen)
        {
            std::cout << "Received ";
            std::cout << static_cast<int>(LMIC.dataLen);
            std::cout << " bytes of payload" << std::endl;
        }
        // Schedule next transmission
        // will be called by main loop
        lorawan_txcomplete = true;  //NKRG
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
        break;
    }
}


void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        std::cout << "OP_TXRXPEND, not sending" << std::endl;
        return;
    }
    else
    {
        // Prepare upstream data transmission at the next possible time.
        std::cout << "Sending: ";
        for (size_t i = 0; i < data_size; i++)
        {
            std::cout << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::endl;
        std::cout << "UplinkSequenceNumber: " << static_cast<uint32_t>(LMIC.seqnoUp) << std::endl;
        LMIC_setTxData2(1, data, data_size, 0); // ,0 NKRG
        // std::cout << "Packet queued" << std::endl;
    }
    // Next TX is scheduled after TX_COMPLETE event.
}


void scheduleSendPayload(u1_t port, u4_t sequenceNo, uint8_t *message, uint8_t n)
{
    data = message;
    data_size = n;
    LMIC.seqnoUp = sequenceNo;

    os_setCallback(&sendjob, do_send);
}


bool getTXcomplete()
{
    return lorawan_txcomplete;
}


void setTXcomplete(bool state)
{
    lorawan_txcomplete = state;
}
