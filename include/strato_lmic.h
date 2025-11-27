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
#include "hal/hal.h"
#include "lmic.h"
// #include "hal/gpio.h"

#define LMIC_CLOCK_ERROR_PPM 30000

void onEvent(void *pUserData, ev_t ev);
bool setup(devaddr_t devaddr, const lmic_pinmap& lmic_pins, unsigned char *appskey, unsigned char *nwkskey);
void do_send(osjob_t* j);
void sendLoraPayload(u1_t port, u4_t sequenceNo, uint8_t* message, uint8_t n);
uint32_t SeqNoFile();
uint8_t PayloadFile(uint8_t* payload);


void dump_rfm96_registers();
uint8_t rfm96_read_register(int spi_fd, uint8_t reg);
static uint8_t *data;
static uint8_t data_size;

extern bool txcomplete;


#endif // __StratoLMIC__