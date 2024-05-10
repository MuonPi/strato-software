#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H




bool init_gpio();

bool set_line_input(int pin);

bool set_line_output(int pin);

bool read_line(int pin);

bool write_line(int pin, int level);




#endif // GPIO_DEVICE_H