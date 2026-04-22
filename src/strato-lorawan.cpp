
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
#include <filesystem>

// #include "hal/hal.h"
// #include "lmic.h"
#include "lmic_lorawan.h"

#include "strato-config.h"
#include "globals.h"
#include "strato-lorawan.h"
#include "CayenneLPP.h"





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



Lorawan::Lorawan(Globals& globals)
    : running(false), StratoGlobals(globals)
{}


Lorawan::~Lorawan()
{
    stop();
}


bool Lorawan::start()
{
    std::cout << "LorawanThread starting ..." << std::endl;
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true))
        return false;

    lorawanThread = std::thread(&Lorawan::threadFunc, this);
    return true;
}


bool Lorawan::stop()
{
    std::cout << "LorawanThread stopping ..." << std::endl;
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false))
        return false;
    if (lorawanThread.joinable())
        lorawanThread.join();
    return true;
}


void Lorawan::threadFunc()
{
    try
    {
        std::cout << "LorawanThread started" << std::endl;

        heartbeat = std::chrono::steady_clock::now();

        const auto start_time = std::chrono::steady_clock::now();
        auto last_message = std::chrono::steady_clock::now();
        const auto interval = std::chrono::seconds(LORAWAN_INTERVAL);
        const auto lorawan_timeout = std::chrono::seconds(LORAWAN_TIMEOUT);
        const auto runloop_interval = std::chrono::milliseconds(RUNLOOP_INTERVAL);

        bool lorawan_inited = false;
        lorawan_inited = init();

        CayenneLPP StratoPayload(255);

        while(running)
        {
            heartbeat = std::chrono::steady_clock::now();

            StratoPayload.reset();
            StratoPayload.addGPS(2, static_cast<float>(StratoGlobals.position[0]), static_cast<float>(StratoGlobals.position[1]), static_cast<float>(StratoGlobals.position[2]));
            StratoPayload.addAnalogInput(3, static_cast<float>(StratoGlobals.voltage_mean));
            StratoGlobals.voltage_mean = 0;
            StratoGlobals.voltage_count = 0;
            StratoPayload.addAnalogInput(4, static_cast<float>(StratoGlobals.XOR_mean));
            StratoGlobals.XOR_mean = 0;
            StratoGlobals.XOR_count = 0;
            StratoPayload.addAnalogInput(5, static_cast<float>(StratoGlobals.AND_mean));
            StratoGlobals.AND_mean = 0;
            StratoGlobals.AND_count = 0;
            StratoPayload.addBarometricPressure(7, static_cast<float>(StratoGlobals.pressure_mean / 100));
            StratoGlobals.pressure_mean = 0;
            StratoGlobals.pressure_count = 0;
            StratoPayload.addTemperature(8, static_cast<float>(StratoGlobals.temperature_mean));
            StratoGlobals.temperature_mean = 0;
            StratoGlobals.temperature_count = 0;


            if (lorawan_inited)
            {
                sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize());
                last_message = std::chrono::steady_clock::now();

                while(true)
                {
                    runloop();
                    if (getTXcomplete() == true)
                        break;
                    if (std::chrono::steady_clock::now() - last_message > lorawan_timeout)
                    {
                        std::cerr << "LORAWAN timeout" << std::endl;
                        lorawan_inited = reset();
                        break;
                    }
                    std::this_thread::sleep_for(runloop_interval);
                }
                setTXcomplete(false);
            }
            else
            {
                lorawan_inited = reset();
            }
            
            std::this_thread::sleep_for(interval - ((std::chrono::steady_clock::now() - start_time) % interval));
        }

        std::cout << "LorawanThread stopped" << std::endl;
        running = false;
    }
    catch(...)
    {
        std::cout << "LorawanThread failed" << std::endl;
        running = false;
    }
}


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

    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);  // NKRG

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



void Lorawan::runloop()
{
    os_runloop_once();
}



bool Lorawan::setup(devaddr_t devaddr, const lmic_pinmap &lmic_pins, unsigned char *appskey, unsigned char *nwkskey)
{
    os_init_ex(&lmic_pins); // LMIC init

    // std::cout << "Starting" << std::endl;
    LMIC_reset(); // Reset the MAC state. Session and pending data transfers will be discarded.
    // network ID 0x01 = Expiremental
    // network ID 0x13 = The Things Network
    LMIC_setSession(0x13, devaddr, nwkskey, appskey);

    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    // LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);   // g2-band

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;
    LMIC.dn2Freq = 869525000;   // NKRG

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


