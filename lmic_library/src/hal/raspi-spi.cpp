#include <chrono>
#include <thread>
#include <iostream>
// #include <bcm2835.h>
#include "raspi-spi.h"


//write manual arduino functions


// int Serial = 1;   // only temporary, should be removed, random value
// int OUTPUT = 1;   // only temporary, should be removed, random value
// int INPUT = 0;    // only temporary, should be removed, random value
// uint8_t MSBFIRST=8;   // only temporary, should be removed, random value
// uint8_t SPI_MODE0=8;  // only temporary, should be removed, random value

// #define Serial 1
// #define OUTPUT 1
// #define INPUT 0
// #define MSBFIRST 8
// #define SPI_MODE0 8


uint32_t micros()
{
  auto global_start_time = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::high_resolution_clock::now() - global_start_time;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  // not completed
}


void delay(s4_t milliseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


void delayMicroseconds(s4_t microseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(microseconds));
}


void pinMode(u1_t x, int y)
{
    std::cout << "pinmode" << std::endl;
}


void digitalWrite(u1_t x, u1_t)
{
    std::cout << "digitalwrite" << std::endl;
}


bool digitalRead(u1_t x)
{
    std::cout << "digitalread" << std::endl;
    return true;
}


void interrupts()
{
  // not possible to enable interrupts on raspi ??
}


void noInterrupts()
{
  // not possible to disable interrupts on raspi ??
}


// void SPISettings::init(uint16_t divider, uint8_t bitOrder, uint8_t dataMode)
// {
//   // something to init SPI Settings
// }


u1_t SPIClass::transfer(u1_t data)
{
  // something to transfer
}

void SPIClass::begin()
{
  // something to begin
}

void SPIClass::beginTransaction(SPISettings settings)
{
  // something to begin transaction
}

void SPIClass::endTransaction()
{
  // something to end transaction
}


