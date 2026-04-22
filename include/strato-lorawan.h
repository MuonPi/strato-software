
#ifndef _STRATO_LORAWAN_H_
#define _STRATO_LORAWAN_H_


// #include "hal/hal.h"
// #include "lmic.h"
#include "lmic_lorawan.h"
#include "globals.h"



class Lorawan
{
public:
    Lorawan(Globals& globals);
    ~Lorawan();

    bool start();
    bool stop();

    std::atomic<bool> running {false};
    std::atomic<std::chrono::steady_clock::time_point> heartbeat {std::chrono::steady_clock::now()};

    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    void runloop();
    bool reset();

    bool setup(devaddr_t devaddr, const lmic_pinmap& lmic_pins, unsigned char *appskey, unsigned char *nwkskey);
    bool SeqNoFile(uint32_t& seqno);

private:
    Globals& StratoGlobals;
    void threadFunc();
    std::thread lorawanThread;
};



#endif // _STRATO_LORAWAN_H_