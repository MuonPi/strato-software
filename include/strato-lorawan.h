
#ifndef _STRATO_LORAWAN_H_
#define _STRATO_LORAWAN_H_


#include <chrono>
#include <QObject>

#include "lmic_lorawan.h"
#include "globals.h"
#include "strato-mqtthandler.h"



class Lorawan : public QObject
{
    Q_OBJECT

public:
    Lorawan(Globals& globals, StratoMqttHandler& mqtt_ref);
    ~Lorawan();
    std::atomic<bool> active = false;
    std::atomic<bool> activated = false;
    std::atomic<bool> inited = false;
    std::chrono::steady_clock::time_point starttime;

public slots:
    bool execute();

private:
    Globals& StratoGlobals;
    StratoMqttHandler& mqtt;
    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    void runloop();
    bool reset();
    bool setup(devaddr_t devaddr, const lmic_pinmap& lmic_pins, unsigned char *appskey, unsigned char *nwkskey);
    bool SeqNoFile(uint32_t& seqno);
    std::string buildUplinkJson(
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
    double temperature_8);
};



#endif // _STRATO_LORAWAN_H_
