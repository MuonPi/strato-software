#ifndef _LORAWAN_H_
#define _LORAWAN_H_


#include "hal/hal.h"
#include "lmic.h"


static uint8_t *data;
static uint8_t data_size;
extern bool lorawan_txcomplete;

void printEvent(ev_t ev);
void onEvent(void *pUserData, ev_t ev);
void do_send(osjob_t* j);
void scheduleSendPayload(u1_t port, u4_t sequenceNo, uint8_t *message, uint8_t n);
bool getTXcomplete();
void setTXcomplete(bool state);


#endif  // _LORAWAN_H_