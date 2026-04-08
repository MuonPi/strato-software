#ifndef RASPI_H
#define RASPI_H

#include <chrono>
#include <thread>
#include <iostream>
#include "../lmic.h"
#include <string>
#include <vector>
#include <cstdint> //Luisa
#include "spidevice.h"
// #include <gpiod.h>


// write manual arduino functions for raspi


#define OUTPUT 1      // only temporary, should be removed, random value
#define INPUT 0       // only temporary, should be removed, random value
#define HIGH 1        // only temporary, should be removed, random value
#define LOW 0         // only temporary, should be removed, random value

#define MSBFIRST 1
#define LSBFIRST 0

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

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


class SerialClass
{
public:
    void begin();
    void println(const std::string& msg);
    void println(const short unsigned int& msg);
    void print(const std::string& msg);
    void print(const char& msg);
    void flush();
};
extern SerialClass Serial;


class SPISettings
{
public:
    uint32_t clock;
    uint8_t bitOrder;
    uint8_t dataMode;

    SPISettings(uint32_t c, uint8_t b, uint8_t m)
        : clock(c), bitOrder(b), dataMode(m) {}
};


class SPIClass
{
public:
    void begin();
    void end();

    void beginTransaction(SPISettings settings);
    void endTransaction();

    uint8_t transfer(uint8_t data);
    void transfer(uint8_t* buf, size_t len);

private:
    spiDevice device;
};
extern SPIClass SPI;


#endif // RASPI_H
