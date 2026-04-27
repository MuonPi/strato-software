#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H

#include <cstdint>
#include <unordered_map>
#include <gpiod.h>



class gpioDevice
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

    #if defined(LIBGPIOD_V2)
    struct PinHandle
    {
        gpiod_line_request* request = nullptr;
    };
    std::unordered_map<unsigned int, PinHandle> pins;

    #elif defined(LIBGPIOD_V1)
    gpiod_line* line = nullptr;
    
    #endif
};



#endif // GPIO_DEVICE_H