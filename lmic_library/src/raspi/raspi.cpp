#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h> //wegen close()
#include "raspi.h"
#include <gpiod.h>
#include "spidevice.h"

// int Serial = 1;   // only temporary, should be removed, random value
// int OUTPUT = 1;   // only temporary, should be removed, random value
// int INPUT = 0;    // only temporary, should be removed, random value
// uint8_t MSBFIRST=8;   // only temporary, should be removed, random value
// uint8_t SPI_MODE0=8;  // only temporary, should be removed, random value

#define MSBFIRST 8
#define SPI_MODE0 8

gpiod_chip *chip{nullptr};
gpiod_line *dio0_line{nullptr};
gpiod_line *reset_line{nullptr};


void delay(s4_t milliseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


void delayMicroseconds(s4_t microseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(microseconds));
}


uint32_t micros()
{
  auto global_start_time = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::high_resolution_clock::now() - global_start_time;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  // not completed
}


void interrupts()
{
  // not possible to enable interrupts on raspi ??
}


void noInterrupts()
{
  // not possible to disable interrupts on raspi ??
}


// ====================================================================================================


bool init_gpio()
{
  chip = gpiod_chip_open("/dev/gpiochip0");
  if (chip == nullptr){
    throw std::runtime_error("could not open gpiochip0");
  }

  dio0_line = gpiod_chip_get_line(chip, DIO_0);
  gpiod_line_set_direction_input(dio0_line);

  reset_line = gpiod_chip_get_line(chip, RESET);
  gpiod_line_set_direction_output(reset_line, 0);
}


void pinMode(u1_t x, int y)
{
    std::cout << "pinmode" << std::endl;
}


void digitalWrite(u1_t pin, u1_t value)
{
  if (pin==RESET && reset_line != nullptr){
    if (gpiod_line_set_value(reset_line, value) != 0){
      throw std::runtime_error("Error trying to write reset line");
    }
  }
}


bool digitalRead(u1_t pin)
{
  if (pin==RESET && reset_line != nullptr){
    return gpiod_line_get_value(dio0_line);
  }
  throw std::runtime_error("Tried to read from unconfigured line");
}


// ====================================================================================================


SPI::spiDevice DEV;

// has to be changed:
std::string devAddress = "/dev/spidev0.0";
int handle = -1;
std::uint32_t speed = 61035;
SPI::Mode mode = SPI::Mode::spi_mode_0;
std::uint8_t bits = 8;


void SPIClass::begin()
{
  DEV.init(devAddress, speed, mode, bits);
}

u1_t SPIClass::transfer(u1_t data)
{
  std::string datastr{};
  datastr  += static_cast<char>(data);
  DEV.write(datastr);
  // something to transfer
}

// const int SCK = 23;   // GPIO11
// const int MOSI = 19;  // GPIO10
// const int SS = 24;    // GPIO8

void SPIClass::beginTransaction(SPISettings settings)
{

}

void SPIClass::endTransaction()
{
  
}