#include <iostream>
#include <cstdint>
#include <gpiod.h>

#include "gpiodevice.h"



gpioDevice::gpioDevice()
{
}



#if defined(LIBGPIOD_V2)


gpioDevice::~gpioDevice()
{
    if (chip)
        gpiod_chip_close(chip);
}


bool gpioDevice::init_gpio()
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (chip == nullptr)
    {
        throw std::runtime_error("Could not open /dev/gpiochip0");
    }
    return true;
}


gpiod_line_request* gpioDevice::set_line_input(unsigned int pin)
{
    gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_config *config = gpiod_line_config_new();
    gpiod_request_config *req_cfg = gpiod_request_config_new();

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    gpiod_line_config_add_line_settings(config, &pin, 1, settings);

    gpiod_line_request *request = gpiod_chip_request_lines(chip, req_cfg, config);

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);
    gpiod_request_config_free(req_cfg);

    if (!request)
    {
        throw std::runtime_error("Could not set line direction input");
    }

    return request;
}


gpiod_line_request* gpioDevice::set_line_output(unsigned int pin)
{
    gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_config *config = gpiod_line_config_new();
    gpiod_request_config *req_cfg = gpiod_request_config_new();

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    gpiod_line_config_add_line_settings(config, &pin, 1, settings);

    gpiod_line_request *request = gpiod_chip_request_lines(chip, req_cfg, config);

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);
    gpiod_request_config_free(req_cfg);

    if (!request)
    {
        throw std::runtime_error("Could not set line direction output");
    }

    return request;
}


bool gpioDevice::read_line(gpiod_line_request* request, unsigned int pin)
{
    return gpiod_line_request_get_value(request, pin);
}


void gpioDevice::write_line(gpiod_line_request* request, unsigned int pin, bool value)
{
    gpiod_line_request_set_value(request, pin, static_cast<enum gpiod_line_value>(value));
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
