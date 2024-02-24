#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <gpiod.h>
#include "raspi.h"
#include "spidevice.h"
#include "lmic.h"
#include "hal.h"

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

bool init_gpio()
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (chip == nullptr)
    {
        throw std::runtime_error("could not open gpiochip0");
    }

    int val{};

    dio0_line = gpiod_chip_get_line(chip, lmic_pins.dio[0]);
    if (dio0_line == nullptr)
    {
        throw std::runtime_error("Could not get line 20");
    }
    val = gpiod_line_request_input(dio0_line, "LoRa DIO0 pin");
    if (val != 0)
    {
        throw std::runtime_error("could not set pin 20 direction input");
    }

    reset_line = gpiod_chip_get_line(chip, lmic_pins.rst);
    if (reset_line == nullptr)
    {
        throw std::runtime_error("Could not get line 20");
    }
    val = gpiod_line_request_output(reset_line, "LoRa Reset", 0);
    if (val != 0)
    {
        throw std::runtime_error("could not set pin 21 direction output");
    }
    usleep(100);
    val = gpiod_line_set_value(reset_line, 1);
    if (val != 0)
    {
        throw std::runtime_error("could not set pin 21 value");
    }
    return true;
}

void pinMode(u1_t pin, int value)
{
    std::cout << "pinmode set pin " << std::dec << static_cast<unsigned>(pin) << " to mode " << static_cast<unsigned>(value) << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (pin != lmic_pins.rst)
    {
        std::cerr << "Tried to write pin " << static_cast<unsigned>(pin) << " to value " << static_cast<unsigned>(value) << " - uninitialized pin, ignored.\n";
        return;
    }
    if (reset_line == nullptr)
    {
        throw std::runtime_error("Nullpointer in reset_line");
    }
    auto val = gpiod_line_set_value(reset_line, value==1 ? 0 : 1);
    if (val != 0)
    {
        throw std::runtime_error("Error trying to write reset line");
    }
}

bool digitalRead(uint8_t pin)
{
    if (pin != lmic_pins.dio[0])
    {
        throw std::runtime_error("Tried to read pin other than dio0");
    }
    if (dio0_line == nullptr)
    {
        throw std::runtime_error("Nullpointer in dio0_line");
    }
    auto val = gpiod_line_get_value(dio0_line);
    if (val < 0)
    {
        throw std::runtime_error("Error trying to read dio0 line");
    }
    return val == 1;
}

// ====================================================================================================
