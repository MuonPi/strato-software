/*******************************************************************************
 * AmbaSat-1
 * Filename: AmbaSatLMIC.h
 *
 * This library is designed for use with AmbaSat-1 and is a wrapper for
 * IBM LMIC functionality
 *
 * Copyright (c) 2021 AmbaSat Ltd
 * https://ambasat.com
 *
 * licensed under Creative Commons Attribution-ShareAlike 3.0
 * ******************************************************************************/

#ifndef __StratoLMIC__
#define __StratoLMIC__

// #include <Wire.h>
#include <lmic.h>
#include <hal/hal.h>
#include "hal/gpio.h"

#define LMIC_CLOCK_ERROR_PPM 30000

class StratoLMIC
{
public:
    bool setup(devaddr_t devaddr, unsigned char *appskey, unsigned char *nwkskey);
    // void sendLoraPayload(u1_t port, u4_t sequenceNo, String &message); // port can be chosen at will
    void sendLoraPayload(u1_t port, u4_t sequenceNo, uint8_t* message, uint8_t n);
    static void do_send(osjob_t *sendjob);
    static void onEvent(void *pUserData, ev_t ev);

private:
    static uint8_t *data;
    static uint8_t data_size;
};

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __StratoLMIC__