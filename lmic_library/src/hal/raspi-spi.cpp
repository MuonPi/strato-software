//Content of this file: SPI access and gpio

#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <cstdint>
#include <unistd.h>
//#include <bcm2835.h>
#include "raspi-spi.h"
//#include "gpio_M.h"
#include <gpiod.h>
// #include <wiringPi.h>
// #include <wiringPiSPI.h>
#include "spidevice.h"

//write manual arduino functions
// static std::unique_ptr<gpio> x_gpio{nullptr};
// static std::shared_ptr<gpio::callback> callback{nullptr};
// gpio my_gpio;

// int Serial = 1;   // only temporary, should be removed, random value
// int OUTPUT = 1;   // only temporary, should be removed, random value
// int INPUT = 0;    // only temporary, should be removed, random value
// uint8_t MSBFIRST=8;   // only temporary, should be removed, random value
// uint8_t SPI_MODE0=8;  // only temporary, should be removed, random value

// #define Serial 1
// #define OUTPUT 1
// #define INPUT 0
#define MSBFIRST 8
#define SPI_MODE0 8

//gpiod_chip *chip{nullptr};
// gpiod_line_request *output_line_request{nullptr};
// gpiod_line_request *input_line_request{nullptr};

uint32_t micros()
{
  auto global_start_time = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::high_resolution_clock::now() - global_start_time;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  // not completed
}

gpiod_chip *chip{nullptr};
gpiod_line *dio0_line{nullptr};
gpiod_line *reset_line{nullptr};
const unsigned DIO_0{20}; // GPIO 20 (input)
const unsigned RESET{21}; // GPIO 21 (output)

bool init_gpio()
{
  chip = gpiod_chip_open("/dev/gpiochip0");
  if (chip == nullptr){
    throw std::runtime_error("could not open gpiochip0");
  }

  dio0_line = gpiod_chip_get_line(chip, DIO_0);
  gpiod_line_set_direction_output(dio0_line, 0);

  reset_line = gpiod_chip_get_line(chip, RESET);
  gpiod_line_set_direction_output(reset_line, 0);
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



void interrupts()
{

}


void noInterrupts()
{
 
}

// void SPISettings::init(uint16_t divider, uint8_t bitOrder, uint8_t dataMode)
// {
//   // something to init SPI Settings
// }

SPI::spiDevice DEV;

std::string devAddress = "/dev/spidev0.0";
int handle = -1;
std::uint32_t speed = 61035;
SPI::Mode mode = SPI::Mode::spi_mode_0;
std::uint8_t bits = 8;
// std::uint8_t* tx;
// std::uint8_t* rx;
// std::uint32_t nBytes = 1;


u1_t SPIClass::transfer(u1_t data)
{
  std::string datastr;
  datastr[0] = data;
  // char datastr = static_cast<char>(data);
  DEV.write(datastr);
}

void SPIClass::begin()
{
  DEV.init(devAddress, speed, mode, bits);
}

void SPIClass::beginTransaction(SPISettings settings)
{
  //überflüssig?
}

void SPIClass::endTransaction()
{
  //überflüssig?
}


