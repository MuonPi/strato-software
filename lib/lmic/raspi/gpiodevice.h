#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H

#include <cstdint>
#include <gpiod.h>




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
#if defined(LIBGPIOD_V2)
    gpiod_line_request* line_request[40] {nullptr};
    gpiod_line_direction line_direction[40] {GPIOD_LINE_DIRECTION_INPUT};
#elif defined(LIBGPIOD_V1)
    gpiod_line* line_request[40] {nullptr};
    #define GPIOD_LINE_DIRECTION_INPUT 1
    #define GPIOD_LINE_DIRECTION_OUTPUT 0
    bool line_direction[40] {GPIOD_LINE_DIRECTION_INPUT};
#endif
};



#endif // GPIO_DEVICE_H