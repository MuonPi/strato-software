
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

#include "hal/hal.h"
#include "lmic.h"

#include "strato-config.h"
#include "strato-lorawan.h"

// bool joined = false;
// bool sleeping = false;

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }
static osjob_t sendjob;
bool txcomplete = 0;
// uint32_t uplinkSequenceNo;



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
        std::cout << "Received ack" << std::endl;
        if (LMIC.dataLen)
        {
            std::cout << "Received ";
            std::cout << static_cast<int>(LMIC.dataLen);
            std::cout << " bytes of payload" << std::endl;
        }
        // Schedule next transmission
        // will be called by main loop
        txcomplete = 1;  //NKRG
        // std::exit(0);   //NKRG
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
        LMIC_setTxData2(1, data, data_size, 0);
        std::cout << "Packet queued" << std::endl;
    }
    // Next TX is scheduled after TX_COMPLETE event.
}




// ========================================================================================================================




Lorawan::Lorawan()
{}


Lorawan::~Lorawan()
{}



bool Lorawan::init()
{
    uint8_t nwkskey[sizeof(NWKSKEY)];
    uint8_t appskey[sizeof(APPSKEY)];

    for (uint8_t i = 0; i < 16; i++)
    {
        nwkskey[i] = NWKSKEY[i];
        appskey[i] = APPSKEY[i];
    }

    setup(DEVADDR, lmic_pins, appskey, nwkskey);

    std::cout << "LORAWAN inited" << std::endl;
    return true;
}



bool Lorawan::sendPayload(uint8_t* payload, uint8_t size)
{
    uint32_t uplinkSequenceNo = 0;
    if(!SeqNoFile(uplinkSequenceNo))
    {
        std::cerr << "could not send payload" << std::endl;
        return false;
    }
    // std::cout << static_cast<int>(uplinkSequenceNo) << std::endl;

    // for (size_t i = 0; i < size; i++)
    // {
    //     std::cout << static_cast<int>(payload[i]) << " ";
    // }
    // std::cout << std::endl;

    scheduleSendPayload(1u, uplinkSequenceNo, payload, size);
    // last_message = std::chrono::steady_clock::now();
    return true;
}



bool Lorawan::runloop()
{
    os_runloop_once();
    // if(txcomplete == true)
    // {
    //     txcomplete = false;
    //     return true;
    // }
    return false;
}



bool Lorawan::setup(devaddr_t devaddr, const lmic_pinmap &lmic_pins, unsigned char *appskey, unsigned char *nwkskey)
{
    os_init_ex(&lmic_pins); // LMIC init

    std::cout << "Starting" << std::endl;
    LMIC_reset(); // Reset the MAC state. Session and pending data transfers will be discarded.
    // network ID 0x01 = Expiremental
    // network ID 0x13 = The Things Network
    LMIC_setSession(0x13, devaddr, nwkskey, appskey);

    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);   // g2-band

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(static_cast<dr_t>(DR_SF10), static_cast<s1_t>(20)); // spreading factor 10

    // // Set data rate and transmit power for uplink
    // LMIC_setDrTxpow(DR_SF7,14);

    LMIC_setAdrMode(true); // adr on

    uint32_t clockError = (LMIC_CLOCK_ERROR_PPM / 100) * (MAX_CLOCK_ERROR / 100) / 100;
    LMIC_setClockError(clockError);

    LMIC_registerEventCb(&onEvent, nullptr);

    // std::cout << "setup end" << std::endl;
    return true;
}



void Lorawan::scheduleSendPayload(u1_t port, u4_t sequenceNo, uint8_t *message, uint8_t n)
{
    data = message;
    data_size = n;
    LMIC.seqnoUp = sequenceNo;

    os_setCallback(&sendjob, do_send);
}



bool Lorawan::SeqNoFile(uint32_t& seqno)
{
    // uint32_t seqno;
    std::string line;
    std::string path;

    path = "/var/strato-software/uplinkSequenceNo.txt";

    if (!std::filesystem::exists(path))
    {
        std::ofstream createfile(path);
        createfile << 0;
        createfile.close();
    }

    std::ifstream readfile(path);
    if (!readfile.is_open())
    {
        std::cerr << "could not open file " << path << std::endl;
        return false;
    }
    std::getline(readfile, line);
    seqno = std::stoul(line);
    readfile.close();

    std::ofstream writefile(path, std::ios::trunc);
    if (!writefile.is_open())
    {
        std::cerr << "could not open file " << path << std::endl;
        return false;
    }
    seqno++;
    writefile << seqno;
    writefile.close();

    // std::cout << seqno << std::endl;

    return true;
}



bool Lorawan::reset()
{
    LMIC_reset();
    std::cout << "LORAWAN resetted" << std::endl;
    return true;
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// #include <iostream>
// #include <iomanip>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/spi/spidev.h>
// #include <cstring>

// #define SPI_DEVICE "/dev/spidev0.0"

// uint8_t rfm96_read_register(int spi_fd, uint8_t reg) {
//     uint8_t tx[2] = { reg & 0x7F, 0x00 }; // MSB=0 → read
//     uint8_t rx[2] = { 0 };

//     struct spi_ioc_transfer tr{};
//     tr.tx_buf = (unsigned long)tx;
//     tr.rx_buf = (unsigned long)rx;
//     tr.len = 2;
//     tr.speed_hz = 8000000;
//     tr.bits_per_word = 8;

//     if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
//         perror("SPI read failed");
//     }

//     return rx[1];
// }

// void dump_rfm96_registers() {
//     int spi_fd = open(SPI_DEVICE, O_RDWR);
//     if (spi_fd < 0) {
//         perror("SPI open failed");
//         return;
//     }

//     uint8_t mode = SPI_MODE_0;
//     uint32_t speed = 8000000;
//     uint8_t bits = 8;

//     ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
//     ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
//     ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

//     std::cout << "RFM96 Register Dump:\n--------------------\n";

//     for (uint8_t reg = 0x00; reg <= 0x70; reg++) {
//         uint8_t val = rfm96_read_register(spi_fd, reg);
//         std::cout << "0x"
//                   << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
//                   << (int)reg << ": 0x"
//                   << std::setw(2) << (int)val << std::dec << "\n";
//     }

//     std::cout << "-------------------- END\n";

//     close(spi_fd);   // SPI sauber freigeben für andere Module
// }
