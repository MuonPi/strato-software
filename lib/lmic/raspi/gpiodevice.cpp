#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <gpiod.h>

#include "gpiodevice.h"



gpioDevice::gpioDevice()
{
}



#if defined(LIBGPIOD_V2)


gpioDevice::~gpioDevice()
{
    for (auto &p : pins)
    {
        if (p.second.request)
            gpiod_line_request_release(p.second.request);
    }

    if (chip)
        gpiod_chip_close(chip);
}


bool gpioDevice::init_gpio()
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (chip == nullptr)
    {
        throw std::runtime_error("could not open gpiochip0");
    }
    return true;
}


bool gpioDevice::set_line_input(unsigned int pin)
{
    auto &p = pins[pin];

    if (p.request)
        return true;

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    gpiod_line_config* cfg = gpiod_line_config_new();

    unsigned int offset = pin;
    gpiod_line_config_add_line_settings(cfg, &offset, 1, settings);

    p.request = gpiod_chip_request_lines(chip, nullptr, cfg);

    gpiod_line_config_free(cfg);
    gpiod_line_settings_free(settings);

    if (!p.request)
        throw std::runtime_error("input failed");

    return true;
}


bool gpioDevice::set_line_output(unsigned int pin)
{
    auto &p = pins[pin];

    if (p.request)
        return true;

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

    gpiod_line_config* cfg = gpiod_line_config_new();

    unsigned int offset = pin;
    gpiod_line_config_add_line_settings(cfg, &offset, 1, settings);

    p.request = gpiod_chip_request_lines(chip, nullptr, cfg);

    gpiod_line_config_free(cfg);
    gpiod_line_settings_free(settings);

    if (!p.request)
        throw std::runtime_error("output failed");

    return true;
}


bool gpioDevice::read_line(unsigned int pin)
{
    auto it = pins.find(pin);
    if (it == pins.end() || !it->second.request)
        throw std::runtime_error("pin not initialized");

    int val = gpiod_line_request_get_value(it->second.request, pin);

    if (val < 0)
        throw std::runtime_error("read failed");

    return val;
}


bool gpioDevice::write_line(unsigned int pin, unsigned int level)
{
    auto it = pins.find(pin);
    if (it == pins.end() || !it->second.request)
        throw std::runtime_error("pin not initialized");

    int ret = gpiod_line_request_set_value(
        it->second.request,
        pin,
        static_cast<gpiod_line_value>(level)
    );

    if (ret < 0)
        throw std::runtime_error("write failed");

    return true;
}


// =================================================================================================================



#elif defined(LIBGPIOD_V1)

gpioDevice::~gpioDevice()
{
    if (line)
    {
        gpiod_line_release(line);
        line = nullptr;
    }

    if (chip)
    {
        gpiod_chip_close(chip);
        chip = nullptr;
    }
}


bool gpioDevice::init_gpio()
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (chip == nullptr)
    {
        throw std::runtime_error("could not open gpiochip0");
    }
    return true;
}


bool gpioDevice::set_line_input(unsigned int pin)
{
    line = gpiod_chip_get_line(chip, pin);
    gpiod_line_release(line);
    if (line == nullptr)
    {
        throw std::runtime_error("Could not get line");
    }
    int val = gpiod_line_request_input(line, "gpiodevice in");     // is it possible to give same names to pins ???
    if (val != 0)
    {
        throw std::runtime_error("Could not set pin direction input");
    }
    return true;
}


bool gpioDevice::set_line_output(unsigned int pin)
{
    line = gpiod_chip_get_line(chip, pin);
    gpiod_line_release(line);
    if (line == nullptr)
    {
        throw std::runtime_error("Could not get line");
    }
    int val = gpiod_line_request_output(line, "gpiodevice out", 0);
    if (val != 0)
    {
        throw std::runtime_error("Could not set pin direction output");
    }
    return true;
}


bool gpioDevice::read_line(unsigned int pin)
{
    line = gpiod_chip_get_line(chip, pin);
    if (line == nullptr)
    {
        throw std::runtime_error("Nullpointer in reading line");
    }
    int val = gpiod_line_get_value(line);
    if (val < 0)
    {
        throw std::runtime_error("Error trying to read line");
    }
    return static_cast<bool>(val);
}


bool gpioDevice::write_line(unsigned int pin, unsigned int level)
{
    line = gpiod_chip_get_line(chip, pin);
    if (line == nullptr)
    {
        throw std::runtime_error("Nullpointer in writing line");
    }
    int val = gpiod_line_set_value(line, level);
    if (val != 0)
    {
        throw std::runtime_error("Error trying to write line");
    }
    return true;
}

#endif
