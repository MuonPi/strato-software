
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

#include "lmic_lorawan.h"

#include "strato-config.h"
#include "globals.h"
#include "strato-lorawan.h"
#include "CayenneLPP.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;





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
    : StratoGlobals(globals)
{}



Lorawan::~Lorawan()
{}



bool Lorawan::execute()
{
    try
    {
        if(!activated)
            return false;
        
        active = true;
        starttime = std::chrono::steady_clock::now();

        // auto now = std::chrono::system_clock::now();
        // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        // std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        // std::tm* tm = std::localtime(&now_c);
        // std::cout << std::put_time(tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count() << std::endl;

        if(inited == false)
        {
            inited = init();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        CayenneLPP StratoPayload(255);

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



        auto msg = buildUplinkJson(
        "eui-70b3d57ed0052abe",
        "marvin5300-arduino-test-0",
        "70B3D57ED0052ABE",
        "2026-06-11T10:56:53.510055778Z",
        StratoGlobals.position[0], 
        StratoGlobals.position[1],
        StratoGlobals.position[2],
        StratoGlobals.voltage_mean,
        StratoGlobals.XOR_mean,
        StratoGlobals.AND_mean,
        StratoGlobals.pressure_mean / 100,
        StratoGlobals.temperature_mean);

        std::cout << msg << std::endl;


        sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize());

        while(getTXcomplete() == false && activated == true)
        {
            runloop();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        setTXcomplete(false);
        active = false;
        return true;
    }
    catch(...)
    {
        std::cerr << "Lorawan execute failed" << std::endl;
        inited = false;
        active = false;
        return false;
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
        std::cerr << "Could not send payload" << std::endl;
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
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }
    std::getline(readfile, line);
    seqno = std::stoul(line);
    readfile.close();

    std::ofstream writefile(path, std::ios::trunc);
    if (!writefile.is_open())
    {
        std::cerr << "Could not open file " << path << std::endl;
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

















std::string Lorawan::buildUplinkJson(
    std::string device_id,
    std::string application_id,
    std::string dev_eui,
    std::string time_iso8601,
    double altitude,
    double latitude,
    double longitude,
    double analog_in_3,
    double analog_in_4,
    double analog_in_5,
    double barometric_pressure_7,
    double temperature_8)
{
    json j;

    j["name"] = "as.up.data.forward";
    j["time"] = time_iso8601;

    j["identifiers"] = {
        {
            {
                "device_ids", {
                    {"device_id", device_id},
                    {"application_ids", {
                        {"application_id", application_id}
                    }},
                    {"dev_eui", dev_eui}
                }
            }
        }
    };

    j["data"] = {
        {"@type", "type.googleapis.com/ttn.lorawan.v3.ApplicationUp"},
        {"end_device_ids", {
            {"device_id", device_id},
            {"application_ids", {
                {"application_id", application_id}
            }},
            {"dev_eui", dev_eui}
        }},
        {"uplink_message", {
            {"decoded_payload", {
                {"analog_in_3", analog_in_3},
                {"analog_in_4", analog_in_4},
                {"analog_in_5", analog_in_5},
                {"barometric_pressure_7", barometric_pressure_7},
                {"gps_2", {
                    {"altitude", altitude},
                    {"latitude", latitude},
                    {"longitude", longitude}
                }},
                {"temperature_8", temperature_8}
            }}
        }}
    };

    return j.dump(2); // pretty print
}










