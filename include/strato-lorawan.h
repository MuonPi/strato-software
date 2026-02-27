
#ifndef _STRATO_LORAWAN_H_
#define _STRATO_LORAWAN_H_

// #include <Wire.h>
#include "hal/hal.h"
#include "lmic.h"
// #include "hal/gpio.h"

#define LMIC_CLOCK_ERROR_PPM 30000

class Lorawan
{
public:
    Lorawan();
    ~Lorawan();

    // bool start();
    // bool stop();

    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    bool runloop();
    bool reset();

    bool setup(devaddr_t devaddr, const lmic_pinmap& lmic_pins, unsigned char *appskey, unsigned char *nwkskey);
    void scheduleSendPayload(u1_t port, u4_t sequenceNo, uint8_t* message, uint8_t n);
    bool SeqNoFile(uint32_t& seqno);
    // uint8_t PayloadFile(uint8_t* payload);

};


static uint8_t *data;
static uint8_t data_size;
extern bool txcomplete;

void printEvent(ev_t ev);
void onEvent(void *pUserData, ev_t ev);
void do_send(osjob_t* j);

void dump_rfm96_registers();
uint8_t rfm96_read_register(int spi_fd, uint8_t reg);


#endif // _STRATO_LORAWAN_H_