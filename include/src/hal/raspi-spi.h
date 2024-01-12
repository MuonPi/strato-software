#include <chrono>
#include <thread>
#include <iostream>
#include "lmic.h"


//write the arduino functions empty
auto start = std::chrono::high_resolution_clock::now();
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

int Serial = 1;
int OUTPUT = 1;
int INPUT = 0;

void pinMode(u1_t x, int y)
{
    printf("pinmode");
}

void digitalWrite(u1_t x, u1_t){
    printf("digitalwrite");
}
bool digitalRead(u1_t x){
    printf("digitalread");
    return true;
}

void interrupts()
{
  // not possible to ensable interrupts on raspi??
}

void noInterrupts()
{
  // not possible to disable interrupts on raspi??
}

uint8_t MSBFIRST=8;
uint8_t SPI_MODE0=8;

// class SPISettings
// {
//  SPISettings(double x, uint8_t y, uint8_t z);
// };

class SPISettings 
{
  public:
    SPISettings(uint16_t divider, uint8_t bitOrder, uint8_t dataMode) {
        // init(divider, bitOrder, dataMode);
    }
    // SPISettings() {
    //     init(BCM2835_SPI_CLOCK_DIVIDER_256, BCM2835_SPI_BIT_ORDER_MSBFIRST, BCM2835_SPI_MODE0);
    // }
  private:
    void init(uint16_t divider, uint8_t bitOrder, uint8_t dataMode) {
    //   this->divider  = divider ; 
    //   this->bitOrder = bitOrder;
    //   this->dataMode = dataMode;
    }

    uint16_t divider  ;
    uint8_t  bitOrder ;
    uint8_t  dataMode ;
  friend class SPIClass;
};

class SPIClass
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

SPIClass SPI;

