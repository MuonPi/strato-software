
#ifndef _STRATO_LORAWAN_H_
#define _STRATO_LORAWAN_H_


#include <QObject>

#include "lmic_lorawan.h"
#include "globals.h"



class Lorawan : public QObject
{
    Q_OBJECT

public:
    Lorawan(Globals& globals);
    ~Lorawan();
    bool running = false;
    bool inited = false;

public slots:
    bool execute();

signals:
    void finished();

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
