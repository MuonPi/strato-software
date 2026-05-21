
#ifndef _STRATO_LORAWAN_H_
#define _STRATO_LORAWAN_H_


#include <chrono>
#include <QObject>

#include "lmic_lorawan.h"
#include "globals.h"



class Lorawan : public QObject
{
    Q_OBJECT

public:
    Lorawan(Globals& globals);
    ~Lorawan();
    std::atomic<bool> active = false;
    std::atomic<bool> activated = false;
    std::atomic<bool> inited = false;
    std::chrono::steady_clock::time_point starttime;

public slots:
    bool execute();

private:
    Globals& StratoGlobals;
    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    void runloop();
    bool reset();
    bool setup(devaddr_t devaddr, const lmic_pinmap& lmic_pins, unsigned char *appskey, unsigned char *nwkskey);
    bool SeqNoFile(uint32_t& seqno);
};



#endif // _STRATO_LORAWAN_H_
