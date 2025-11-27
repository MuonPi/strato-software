#ifndef RASPI_H
#define RASPI_H

#include <chrono>
#include <thread>
#include <iostream>
#include "../lmic.h"
#include <string>
#include <vector>
#include <cstdint> //Luisa
#include <gpiod.h>


// write manual arduino functions for raspi


#define OUTPUT 1      // only temporary, should be removed, random value
#define INPUT 0       // only temporary, should be removed, random value
#define MSBFIRST 0    // only temporary, should be removed, random value
#define SPI_MODE0 0   // only temporary, should be removed, random value
#define HIGH 1        // only temporary, should be removed, random value
#define LOW 0         // only temporary, should be removed, random value

// const unsigned DIO_0{20}; // GPIO 20 (input)
// const unsigned RESET{21}; // GPIO 21 (output)

// static gpio m_gpio;

uint32_t micros();
uint32_t millis();

void delay(uint32_t milliseconds);
void delayMicroseconds(uint32_t microseconds);

bool init_gpio();
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
bool digitalRead(uint8_t pin);

void interrupts();
void noInterrupts();

#endif // RASPI_H
