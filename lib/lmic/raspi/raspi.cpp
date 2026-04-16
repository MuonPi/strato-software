#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h>
// #include <gpiod.h>
#include <stdint.h>
#include <time.h>
#include "raspi.h"
#include "spidevice.h"
#include "gpiodevice.h"
#include "lmic.h"
#include "hal.h"



bool gpio_init_state = 0;


void delay(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void delayMicroseconds(uint32_t microseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(microseconds));
}

uint32_t micros()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    uint64_t us = (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;

    return (uint32_t)(us & 0xFFFFFFFFULL);
}

uint32_t millis()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    uint64_t ms = (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;

    return (uint32_t)ms;
}

void interrupts()
{
    // not possible to enable interrupts on raspi
}

void noInterrupts()
{
    // not possible to disable interrupts on raspi
}

void SerialClass::begin()
{}

void SerialClass::println(const std::string& msg)
{
    std::cout << msg << std::endl;
}

void SerialClass::println(const short unsigned int& msg)
{
    std::cout << msg << std::endl;
}

void SerialClass::print(const std::string& msg)
{
    std::cout << msg;
}

void SerialClass::print(const char& msg)
{
    std::cout << msg << std::endl;
}

void SerialClass::flush()
{
    std::cout.flush();
}

SerialClass Serial;


// ====================================================================================================


void pinMode(uint8_t pin, uint8_t mode)
{
    // std::cout << "pinmode set pin " << std::dec << static_cast<unsigned>(pin) << " to mode " << static_cast<unsigned>(mode) << std::endl;
    if(gpio_init_state == 0)
    {
        init_gpio();
        gpio_init_state = 1;
    }
    if(mode == INPUT)
        set_line_input(pin);
    else if(mode == OUTPUT)
        set_line_output(pin);
}

bool digitalRead(uint8_t pin)
{
    if(gpio_init_state == 0)
    {
        init_gpio();
        gpio_init_state = 1;
    }
    pinMode(pin, INPUT);
    return read_line(pin);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if(gpio_init_state == 0)
    {
        init_gpio();
        gpio_init_state = 1;
    }
    pinMode(pin, OUTPUT);
    write_line(pin, value);
}


// ====================================================================================================


void SPIClass::begin()
{
    std::cout << "spi begin" << std::endl;
    device.init("/dev/spidev0.0", 1000000, Mode::spi_mode_0, 8);
}

void SPIClass::end()
{
}

void SPIClass::beginTransaction(SPISettings settings)
{
    Mode mode = Mode::spi_mode_0;

    switch(settings.dataMode)
    {
        case SPI_MODE0: mode = Mode::spi_mode_0; break;
        case SPI_MODE1: mode = Mode::spi_mode_1; break;
        case SPI_MODE2: mode = Mode::spi_mode_2; break;
        case SPI_MODE3: mode = Mode::spi_mode_3; break;
    }

    device.init("/dev/spidev0.0", settings.clock, mode, 8);
}

void SPIClass::endTransaction()
{
}

uint8_t SPIClass::transfer(uint8_t data)
{
    std::string tx;
    tx.push_back(data);

    device.write(0, tx);     // send data
    auto rx = device.read(0,1);

    if(rx.size())
        return rx[0];

    return 0;
}

void SPIClass::transfer(uint8_t* buf, size_t len)
{
    for(size_t i=0;i<len;i++)
        buf[i] = transfer(buf[i]);
}

SPIClass SPI;