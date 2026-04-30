#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H

#include <cstdint>
#include <gpiod.h>




#if defined(LIBGPIOD_V2)

class gpioDevice
{
public:
    gpioDevice();
    ~gpioDevice();
    bool init_gpio();
    void set_line_input(unsigned int pin);
    void set_line_output(unsigned int pin);
    bool read_line(unsigned int pin);
    void write_line(unsigned int pin, bool value);

private:
    gpiod_chip* chip = nullptr;
    gpiod_line_request* line_request[40] {nullptr};
    gpiod_line_direction line_direction[40] {GPIOD_LINE_DIRECTION_INPUT};
};


#elif defined(LIBGPIOD_V1)


class gpioDevice_V1
{
public:
    gpioDevice();
    ~gpioDevice();
    bool init_gpio();
    bool set_line_input(unsigned int pin);
    bool set_line_output(unsigned int pin);
    bool read_line(unsigned int pin);
    bool write_line(unsigned int pin, unsigned int level);

private:
    gpiod_chip* chip = nullptr;
    gpiod_line* line = nullptr;
};

#endif

#endif // GPIO_DEVICE_H