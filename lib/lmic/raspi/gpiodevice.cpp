#include <iostream>
#include <cstdint>
#include <gpiod.h>

#include "gpiodevice.h"



gpioDevice::gpioDevice()
{
}


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




#if defined(LIBGPIOD_V2)


void gpioDevice::set_line_input(unsigned int pin)
{
    if(line_request[pin] != nullptr)
    {
        if(line_direction[pin] == GPIOD_LINE_DIRECTION_INPUT)
            return;
        else
        {
            gpiod_line_request_release(line_request[pin]);
            line_request[pin] = nullptr;
        }
    }

    gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_config *config = gpiod_line_config_new();
    gpiod_request_config *req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "strato-software");

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_config_add_line_settings(config, &pin, 1, settings);
    line_request[pin] = gpiod_chip_request_lines(chip, req_cfg, config);
    line_direction[pin] = GPIOD_LINE_DIRECTION_INPUT;

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);
    gpiod_request_config_free(req_cfg);

    if (!line_request[pin])
    {
        throw std::runtime_error("Could not set line direction input");
    }

    return;
}


void gpioDevice::set_line_output(unsigned int pin)
{
    if(line_request[pin] != nullptr)
    {
        if(line_direction[pin] == GPIOD_LINE_DIRECTION_OUTPUT)
            return;
        else
        {
            gpiod_line_request_release(line_request[pin]);
            line_request[pin] = nullptr;
        }
    }

    gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_config *config = gpiod_line_config_new();
    gpiod_request_config *req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "strato-software");

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_config_add_line_settings(config, &pin, 1, settings);
    line_request[pin] = gpiod_chip_request_lines(chip, req_cfg, config);
    line_direction[pin] = GPIOD_LINE_DIRECTION_OUTPUT;

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);
    gpiod_request_config_free(req_cfg);

    if (!line_request[pin])
    {
        throw std::runtime_error("Could not set line direction output");
    }

    return;
}


bool gpioDevice::read_line(unsigned int pin)
{
    if(line_request[pin] == nullptr || line_direction[pin] != GPIOD_LINE_DIRECTION_INPUT)
        set_line_input(pin);
    return static_cast<bool>(gpiod_line_request_get_value(line_request[pin], pin));
}


void gpioDevice::write_line(unsigned int pin, bool value)
{
    if(line_request[pin] == nullptr || line_direction[pin] != GPIOD_LINE_DIRECTION_OUTPUT)
        set_line_output(pin);
    gpiod_line_request_set_value(line_request[pin], pin, static_cast<enum gpiod_line_value>(value));
}


// =================================================================================================================



#elif defined(LIBGPIOD_V1)


void gpioDevice::set_line_input(unsigned int pin)
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


void gpioDevice::set_line_output(unsigned int pin)
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


void gpioDevice::write_line(unsigned int pin, bool value)
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
