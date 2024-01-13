#include <chrono>
#include <thread>
#include <iostream>
// #include "bcm2835.h"
#include "lmic.h"
#include "raspi-spi.h"



//write manual arduino functions


uint32_t micros()
{
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
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


u1_t SPIClass::transfer(u1_t _data)
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
