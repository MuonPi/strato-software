#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <gpiod.h>

#include "raspi.h"
#include "spidevice.h"
#include "gpiodevice.h"
#include "lmic.h"
#include "hal.h"



static const Arduino_LMIC::HalPinmap_t *plmic_pins = &lmic_pins;


bool gpio_init_state = 0;
bool spi_init_state = 0;



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


// =========================================== GPIO =========================================================


gpioDevice lmicGpioDevice;

void pinMode(uint8_t pin, uint8_t mode)
{
    // std::cout << "pinMode" << std::endl;
    if (pin == plmic_pins->nss)
        return;
    if(gpio_init_state == 0)
    {
        lmicGpioDevice.init_gpio();
        gpio_init_state = 1;
    }
    if(mode == INPUT)
        lmicGpioDevice.set_line_input(static_cast<unsigned int>(pin));
    else if(mode == OUTPUT)
        lmicGpioDevice.set_line_output(static_cast<unsigned int>(pin));
    // std::cout << "pinMode set pin " << std::dec << static_cast<unsigned>(pin) << " to mode " << static_cast<unsigned>(mode) << std::endl;
    return;
}

bool digitalRead(uint8_t pin)
{
    // std::cout << "digitalRead" << std::endl;
    if(pin == plmic_pins->nss)
        return 0;
    if(gpio_init_state == 0)
    {
        lmicGpioDevice.init_gpio();
        gpio_init_state = 1;
    }
    return lmicGpioDevice.read_line(static_cast<unsigned int>(pin));
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    // std::cout << "digitalWrite" << std::endl;
    if (pin == plmic_pins->nss)
        return;
    if(gpio_init_state == 0)
    {
        lmicGpioDevice.init_gpio();
        gpio_init_state = 1;
    }
    lmicGpioDevice.write_line(static_cast<unsigned int>(pin), static_cast<bool>(value));
    return;
}


// =========================================== SPI =========================================================


spiDevice lmicSpiDevice;

void SPIClass::begin()
{
    // std::cout << "spi begin" << std::endl;
    lmicSpiDevice.init("/dev/spidev0.0");
}

void SPIClass::end()
{
}

void SPIClass::beginTransaction(SPISettings settings)
{
    if(spi_init_state == 1)
        return;

    Mode mode = Mode::spi_mode_0;

    switch(settings.dataMode)
    {
        case SPI_MODE0: mode = Mode::spi_mode_0; break;
        case SPI_MODE1: mode = Mode::spi_mode_1; break;
        case SPI_MODE2: mode = Mode::spi_mode_2; break;
        case SPI_MODE3: mode = Mode::spi_mode_3; break;
    }

    lmicSpiDevice.configure(settings.clock, mode, 8);
    
    spi_init_state = 1;
}

void SPIClass::endTransaction()
{
}

uint8_t SPIClass::transfer(uint8_t data)
{
    // std::cout << "spi transfer: " << data << std::endl;

    std::string tx;
    tx.push_back(data);

    lmicSpiDevice.write(tx);     // send data
    auto rx = lmicSpiDevice.read(0, 1);

    // std::cout << "spi receive: " << rx << std::endl;

    if(rx.size())
        return rx[0];

    return 0;
}


void SPIClass::transfer(uint8_t cmd, uint8_t* buf, size_t count, bit_t is_read)
{
    // std::cout << "spi transfer: " << count << " bytes" << std::endl;

    std::string buf_str;
    buf_str.resize(count);

    for(size_t i = 0; i < count; i++)
        buf_str[i] = buf[i];

    if (is_read)
    {
        buf_str = lmicSpiDevice.read(cmd, count);

        for(size_t i = 0; i < count; i++)
            buf[i] = static_cast<uint8_t>(buf_str[i]);
    }
    else
    {
        lmicSpiDevice.write(cmd, buf_str);

        for(size_t i = 0; i < count; i++)
            buf[i] = 0x00;
    }
    return;
}



SPIClass SPI;
