#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <gpiod.h>
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
    // auto global_start_time = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::high_resolution_clock::now() - global_start_time;
    // return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    // not completed
    return 0;
}

uint32_t millis()
{
    return 0;
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

void pinMode(uint8_t pin, uint8_t mode)
{
    std::cout << "pinmode set pin " << std::dec << static_cast<unsigned>(pin) << " to mode " << static_cast<unsigned>(mode) << std::endl;
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
    std::cout << "digital Read" << std::endl;
    return read_line(pin);
    // std::cout << "after read line" << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    std::cout << "digital Write" << std::endl;
    write_line(pin, value);
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

// ====================================================================================================
