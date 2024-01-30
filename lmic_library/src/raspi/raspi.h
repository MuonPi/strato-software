#ifndef RASPI_H
#define RASPI_H

#include <chrono>
#include <thread>
#include <iostream>
//#include <bcm2835.h>
// #include <bcm2835.h>
#include "../lmic.h"
#include <string>
#include <vector>
#include <cstdint> //Luisa
#include "gpio_M.h"
#include <gpiod.h>


//write manual arduino functions


// int Serial; // = 1;   // only temporary, should be removed, random value
// int OUTPUT; // = 1;   // only temporary, should be removed, random value
// int INPUT; // = 0;    // only temporary, should be removed, random value
// uint8_t MSBFIRST; // = 8;   // only temporary, should be removed, random value
// uint8_t SPI_MODE0; // = 8;  // only temporary, should be removed, random value

#define Serial 1
#define OUTPUT 1
#define INPUT 0
 #define MSBFIRST 8
 #define SPI_MODE0 8
#define HIGH 1
#define LOW 0
const unsigned DIO_0{20}; // GPIO 20 (input)
const unsigned RESET{21}; // GPIO 21 (output)

// static gpio m_gpio;

uint32_t micros();

void delay(s4_t milliseconds);
void delayMicroseconds(s4_t microseconds);

void pinMode(u1_t x, int y);
void digitalWrite(u1_t pin, u1_t value);
bool digitalRead(u1_t x);

 void interrupts();
 void noInterrupts();

class SPISettings
{
  public:
    SPISettings(uint16_t divider, uint8_t bitOrder, uint8_t dataMode)
    {
        // init(divider, bitOrder, dataMode);
    }
    // SPISettings() {
    //     init(BCM2835_SPI_CLOCK_DIVIDER_256, BCM2835_SPI_BIT_ORDER_MSBFIRST, BCM2835_SPI_MODE0);
    // }
  private:
    void init(uint16_t divider, uint8_t bitOrder, uint8_t dataMode)
    {
    //   this->divider  = divider ; 
    //   this->bitOrder = bitOrder;
    //   this->dataMode = dataMode;
    }

    // uint16_t divider  ;
    // uint8_t  bitOrder ;
    // uint8_t  dataMode ;
  friend class SPIClass;
};

class SPIClass  // other structure instead of SPISettings and SPIClass ??
{
  public:
    static u1_t transfer(u1_t _data);
    // SPI Configuration methods
    static void begin(); // Default
    // static void end();
    static void beginTransaction(SPISettings settings);
    static void endTransaction();
    // static void setBitOrder(uint8_t);
    // static void setDataMode(uint8_t);
    // static void setClockDivider(uint16_t);
};

#endif // RASPI_H