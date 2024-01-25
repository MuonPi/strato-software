#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
// #include <bcm2835.h>
#include "raspi-spi.h"
#include "gpio.h"

//write manual arduino functions
static std::unique_ptr<gpio> x_gpio{nullptr};
static std::shared_ptr<gpio::callback> callback{nullptr};
gpio my_gpio;

// int Serial = 1;   // only temporary, should be removed, random value
// int OUTPUT = 1;   // only temporary, should be removed, random value
// int INPUT = 0;    // only temporary, should be removed, random value
// uint8_t MSBFIRST=8;   // only temporary, should be removed, random value
// uint8_t SPI_MODE0=8;  // only temporary, should be removed, random value

// #define Serial 1
// #define OUTPUT 1
// #define INPUT 0
// #define MSBFIRST 8
// #define SPI_MODE0 8

//gpiod_chip *chip{nullptr};
// gpiod_line_request *output_line_request{nullptr};
// gpiod_line_request *input_line_request{nullptr};

// uint32_t micros()
// {
//   auto global_start_time = std::chrono::high_resolution_clock::now();
//   auto elapsed = std::chrono::high_resolution_clock::now() - global_start_time;
//   return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//   // not completed
// }

// gpiod_line_set_flags* default_output_low() {
//   gpiod_line_settings *default_output = gpiod_line_settings_new();
//   gpiod_line_settings_set_direction(default_output, gpiod_line_direction::GPIOD_LINE_DIRECTION_OUTPUT);
//   gpiod_line_settings_set_drive(default_output, gpiod_line_drive::GPIOD_LINE_DRIVE_PUSH_PULL);
//   gpiod_line_settings_set_output_value(default_output, gpiod_line_value::GPIOD_LINE_VALUE_INACTIVE);
//   return default_output;
// }
// gpiod_line_set_flags* default_output_high() {
//   gpiod_line_settings *default_output = gpiod_line_settings_new();
//   gpiod_line_settings_set_direction(default_output, gpiod_line_direction::GPIOD_LINE_DIRECTION_OUTPUT);
//   gpiod_line_settings_set_drive(default_output, gpiod_line_drive::GPIOD_LINE_DRIVE_PUSH_PULL);
//   gpiod_line_settings_set_output_value(default_output, gpiod_line_value::GPIOD_LINE_VALUE_ACTIVE);
//   return default_output;
// }

bool init_gpio()
{
  x_gpio = std::make_unique<gpio>();
  gpio::setting pin_setting{};
  pin_setting.gpio_pins={20};
  callback = x_gpio->list_callback(pin_setting);
  x_gpio->start();

  // chip = gpiod_chip_open("/dev/gpiochip0");
  // if (chip == nullptr){
  //   throw std::runtime_error("could not open gpiochip0");
  // }

  // constexpr unsigned int n_output_pins{3};
  // const unsigned int output_pins[n_output_pins] = {1u, 13u, 23u};

  // gpiod_line_request_config *request_config = gpiod_request_config_new();
  // gpio_v2_line_config *line_config = gpiod_line_iter_new();
  // gpiod_line_set_flags *default_output = gpiod_line_iter_new();
  // gpiod_line_config_add_line_settings(line_config, output_pins, n_output_pins, default_output_low());
  // gpiod_line_request = gpiod_chip_get_lines(chip, request_config, line_config);
  // if (gpiod_line_request == nullptr){
  //   throw std::runtime_error("could not request lines");
  // }

  // constexpr unsigned int n_input_pins{3};
  // const unsigned int input_pins[n_input_pins] = {2u, 14u, 24u};

  // // gpiod_request_config *request_config = gpiod_request_config_new();
  // // gpiod_line_config *line_config = gpiod_line_config_new();
  // // gpiod_line_settings *default_output = gpiod_line_settings_new();
  // // gpiod_line_config_add_line_settings(line_config, output_pins, n_output_pins, default_output_low());
  // // output_line_request = gpiod_chip_request_lines(chip, request_config, line_config);
  // if (gpiod_line_request == nullptr){
  //   throw std::runtime_error("could not request lines");
  // }
}

void delay(s4_t milliseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


void delayMicroseconds(s4_t microseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(microseconds));
}


void pinMode(u1_t x, int y)
{
    std::cout << "pinmode" << std::endl;
}


void digitalWrite(u1_t pin, u1_t value)
{
  u1_t x = 8;
     std::cout << "digitalwrite" << std::endl;
     my_gpio.write(x);
//     unsigned int offsets[1] = {pin};
//     gpiod_line_name line_value = gpiod_line_value::GPIOD_LINE_VALUE_INACTIVE;
//     if (value == 1){
//       line_value = gpiod_line_value::GPIOD_LINE_VALUE_ACTIVE;
//     }
//     gpiod_line_name va_list[1] = {line_value};
//     if (!gpiod_line_request_set_values_subset(gpiod_line_request, 1u, offsets, va_list)){
//       throw std::runtime_error(std::string("error setting pin to value"));
//     }
}


bool digitalRead(u1_t x)
{
  // Wie in digitalWrite
    std::cout << "digitalread" << std::endl;

    return true;
}


void interrupts()
{
  // not possible to enable interrupts on raspi ??
}


void noInterrupts()
{
  //  int mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
  //   if (mem_fd < 0) {
  //       std::cerr << "Error opening /dev/mem" << std::endl;
  //       return 1;
  //   }

  //   void* gpio_map = mmap(
  //       NULL,             // Any adddress in our space will do
  //       4096,             // Map length
  //       PROT_READ|PROT_WRITE, // Enable reading & writting to mapped memory
  //       MAP_SHARED,       // Shared with other processes
  //       mem_fd,           // File to map
  //       GPIO_BASE         // Offset to GPIO peripheral
  //   );

  //   close(mem_fd); // No need to keep mem_fd open after mmap

  //   if (gpio_map == MAP_FAILED) {
  //       std::cerr << "Error mapping the physical address" << std::endl;
  //       return 1;
  //   }

  //   // Set the pin as input (for example, GPIO pin 17)
  //   volatile unsigned int* gpio = (volatile unsigned int*)gpio_map;
  //   *(gpio + GPFSEL0/4) &= ~(7<<21);  // Clear bits for pin 17

  //   // Disable edge detection (for example, GPIO pin 17)
  //   *(gpio + GPREN0/4) &= ~(1<<17);  // Disable rising edge detection
  //   *(gpio + GPFEN0/4) &= ~(1<<17);  // Disable falling edge detection

   
}


// void SPISettings::init(uint16_t divider, uint8_t bitOrder, uint8_t dataMode)
// {
//   // something to init SPI Settings
// }


u1_t SPIClass::transfer(u1_t data)
{
  // something to transfer
}

void SPIClass::begin()
{
  // something to begin
}

void SPIClass::beginTransaction(SPISettings settings)
{
  // something to begin transaction
}

void SPIClass::endTransaction()
{
  // something to end transaction
}


