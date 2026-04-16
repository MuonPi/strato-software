#include <iostream>
#include <unistd.h>
#include <gpiod.h>



gpiod_chip *chip{nullptr};
// gpiod_line *dio0_line{nullptr};
// gpiod_line *reset_line{nullptr};
gpiod_line *line{nullptr};



bool init_gpio()
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (chip == nullptr)
    {
        throw std::runtime_error("could not open gpiochip0");
    }
    return 0;
}



bool set_line_input(int pin)
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
        throw std::runtime_error("could not set pin direction input");
    }
    return 0;
}



bool set_line_output(int pin)
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
        throw std::runtime_error("could not set pin direction output");
    }
    return 0;
}



bool read_line(int pin)
{
    // std::cout << "digital Read line " << std::dec << static_cast<int>(pin) << std::endl;
    line = gpiod_chip_get_line(chip, pin);
    if (line == nullptr)
    {
        throw std::runtime_error("Nullpointer in reading line");
    }
    if (gpiod_line_direction(line) != GPIOD_LINE_DIRECTION_INPUT)
        set_line_input(pin);
    int val = gpiod_line_get_value(line);
    if (val < 0)
    {
        throw std::runtime_error("Error trying to read line");
    }
    // std::cout << "Read Value: " << static_cast<bool>(val) << std::endl;
    return static_cast<bool>(val);
}



bool write_line(int pin, int level)
{
    line = gpiod_chip_get_line(chip, pin);
    if (line == nullptr)
    {
        throw std::runtime_error("Nullpointer in writing line");
    }
    if (gpiod_line_direction(line) != GPIOD_LINE_DIRECTION_OUTPUT)
        set_line_output(pin);
    int val = gpiod_line_set_value(line, level); // level == 1 ? 0 : 1);
    if (val != 0)
    {
        throw std::runtime_error("Error trying to write line");
    }
    return 0;
}