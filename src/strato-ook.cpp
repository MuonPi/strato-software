#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "CayenneLPP.h"
#include "ook/ook_transmitter.h"
#include "strato-config.h"
#include "strato-ook.h"

Ook::Ook(Globals& globals)
    : StratoGlobals(globals)
{}

Ook::~Ook()
{}

bool Ook::execute()
{
    try
    {
        if(!activated)
            return false;

        active = true;
        starttime = std::chrono::steady_clock::now();

        if(inited == false)
        {
            inited = init();
            if(inited == false)
            {
                active = false;
                return false;
            }
        }

#if OOK_PAYLOAD_FORMAT == OOK_PAYLOAD_FORMAT_TEXT
        if(!sendTextPayload(buildTextPayload()))
#elif OOK_PAYLOAD_FORMAT == OOK_PAYLOAD_FORMAT_CAYENNE_LPP
        CayenneLPP StratoPayload(255);

        StratoPayload.reset();
        StratoPayload.addGPS(2, static_cast<float>(StratoGlobals.position[0]), static_cast<float>(StratoGlobals.position[1]), static_cast<float>(StratoGlobals.position[2]));
        StratoPayload.addAnalogInput(3, static_cast<float>(StratoGlobals.voltage_mean));
        StratoPayload.addAnalogInput(4, static_cast<float>(StratoGlobals.XOR_mean));
        StratoPayload.addAnalogInput(5, static_cast<float>(StratoGlobals.AND_mean));
        StratoPayload.addBarometricPressure(7, static_cast<float>(StratoGlobals.pressure_mean / 100));
        StratoPayload.addTemperature(8, static_cast<float>(StratoGlobals.temperature_mean));
        StratoPayload.addAnalogInput(9, static_cast<float>(StratoGlobals.ltr390_uv_mean));

        if(!sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize()))
#else
#error "Unsupported OOK_PAYLOAD_FORMAT"
#endif
        {
            inited = false;
            active = false;
            return false;
        }

        active = false;
        return true;
    }
    catch(...)
    {
        std::cerr << "OOK execute failed" << std::endl;
        inited = false;
        active = false;
        return false;
    }
}

bool Ook::init()
{
    MuonPi::Ook::Transmitter::Config config{};
    config.gpio = OOK_GPIO_PIN;
    config.halfBitPeriod = std::chrono::microseconds(OOK_HALF_BIT_US);
    config.repeatGap = std::chrono::milliseconds(OOK_REPEAT_GAP_MS);
    config.repeats = OOK_REPEATS;

    transmitter = std::make_unique<MuonPi::Ook::Transmitter>(config);
    if(!transmitter->open())
    {
        transmitter.reset();
        std::cerr << "Could not init OOK" << std::endl;
        return false;
    }

    std::cout << "OOK inited" << std::endl;
    return true;
}

bool Ook::sendPayload(uint8_t* payload, uint8_t size)
{
    if(transmitter == nullptr)
    {
        std::cerr << "Could not send OOK payload" << std::endl;
        return false;
    }

    std::vector<std::uint8_t> payload_bytes(payload, payload + size);
    if(!transmitter->sendBytes(payload_bytes))
    {
        std::cerr << "Could not send OOK payload" << std::endl;
        return false;
    }

    return true;
}

bool Ook::sendTextPayload(const std::string& payload)
{
    if(transmitter == nullptr)
    {
        std::cerr << "Could not send OOK text payload" << std::endl;
        return false;
    }

    if(!transmitter->sendString(payload))
    {
        std::cerr << "Could not send OOK text payload" << std::endl;
        return false;
    }

    return true;
}

std::string Ook::buildTextPayload() const
{
    std::ostringstream payload;
    payload << "lat=" << static_cast<double>(StratoGlobals.position[0])
            << ";lon=" << static_cast<double>(StratoGlobals.position[1])
            << ";alt=" << static_cast<double>(StratoGlobals.position[2])
            << ";v=" << static_cast<double>(StratoGlobals.voltage_mean)
            << ";xor=" << static_cast<double>(StratoGlobals.XOR_mean)
            << ";and=" << static_cast<double>(StratoGlobals.AND_mean)
            << ";p=" << static_cast<double>(StratoGlobals.pressure_mean) / 100
            << ";t=" << static_cast<double>(StratoGlobals.temperature_mean)
            << ";uv=" << static_cast<double>(StratoGlobals.ltr390_uv_mean);
    return payload.str();
}
