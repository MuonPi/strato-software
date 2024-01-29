#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <unistd.h> //wegen close()
//#include <cstdint>
//#include <bcm2835.h>
#include "raspi-spi.h"
//#include "gpio_M.h"
#include <gpiod.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

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
#define MSBFIRST 8
#define SPI_MODE0 8

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
  // x_gpio = std::make_unique<gpio>();
  // gpio::setting pin_setting{};
  // pin_setting.gpio_pins={20};
  // callback = x_gpio->list_callback(pin_setting);
  // x_gpio->start();

  // chip = gpiod_chip_open("/dev/gpiochip0");
  // if (chip == nullptr){
  //   throw std::runtime_error("could not open gpiochip0");
  // }

  // constexpr unsigned int n_output_pins{3};
  // const unsigned int output_pins[n_output_pins] = {1u, 13u, 23u};

  // gpiod_line_request_config *request_config = gpiod_request_config_new();
  // gpiod_line *line_config = gpiod_line_iter_new();
  // gpiod_line_set_flags *default_output = gpiod_line_iter_new();
  // // gpiod_line_config_add_line_settings(line_config, output_pins, n_output_pins, default_output_low()); //luisa
  // gpiod_line_set_config(line_config, output_pins*, n_output_pins, nullptr);
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

// const int SCK = 23;   // GPIO11
// const int MOSI = 19;  // GPIO10
// const int SS = 24;    // GPIO8

// #define GPIO_BASE_ADDRESS 0x3F200000

// void SPIClass::begin()
// {

//  // Set SCK, MOSI, and SS pins as outputs
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x04) |= (1 << 6);  // SCK as output
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x04) |= (1 << 3);  // MOSI as output
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x00) |= (1 << 24);  // SS as output

//     // Pull SCK and MOSI low, and SS high
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x08) &= ~(1 << 6);  // Clear SCK bit
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x08) &= ~(1 << 3);  // Clear MOSI bit
//     *(volatile uint32_t*)(GPIO_BASE_ADDRESS + 0x04) |= (1 << 24);   // Set SS bit high
// } //to directly manipulate the registers

void SPIClass::begin()
{
  wiringPiSetup();  // Initialize the wiringPi library

    // Set SPI pins as outputs
    pinMode(11, OUTPUT);  // SCK as output (wiringPi pin 14)
    pinMode(10, OUTPUT);  // MOSI as output (wiringPi pin 12)
    pinMode(8, OUTPUT);  // SS as output (wiringPi pin 10)

    // Pull SCK and MOSI low, and SS high
    digitalWrite(11, LOW);   // Pull SCK low
    digitalWrite(10, LOW);   // Pull MOSI low
    digitalWrite(8, HIGH);  // Pull SS high

} //using wiringpi

// void SPIClass::begin()
// {
//   if (!bcm2835_init()) {
//         // Handle initialization error
//     }

//     // Set SPI pins as outputs
//     bcm2835_gpio_fsel(RPI_V2_GPIO_P1_23, BCM2835_GPIO_FSEL_OUTP);  // SCK as output
//     bcm2835_gpio_fsel(RPI_V2_GPIO_P1_19, BCM2835_GPIO_FSEL_OUTP);  // MOSI as output
//     bcm2835_gpio_fsel(RPI_V2_GPIO_P1_24, BCM2835_GPIO_FSEL_OUTP);  // SS as output

//     // Additional configuration can be added here, such as setting the clock frequency

//     // Initialize SPI
//     bcm2835_spi_begin();
// } //using the bcm2835


// void SPIClass::beginTransaction(SPISettings settings)
// {
//  // Set SPI configuration
//     bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // Set MSB first
//     bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); // Set SPI mode 0
//     bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); // Set clock speed (change as needed)
//     bcm2835_spi_chipSelect(BCM2835_SPI_CS0); // Set chip select}
// } //using bcm2835

void SPIClass::beginTransaction(SPISettings settings)
{
  wiringPiSPISetupMode(0, 1000000, 0);  // Set up SPI channel 0 with a speed of 1,000,000 Hz and mode 0
} //using wiringpi

// void SPIClass::endTransaction()
// {
//     bcm2835_spi_end();
// } //using bcm2835

void SPIClass::endTransaction()
{
  //was genau muss diese Funktion können?
  close(wiringPiSPIGetFd(0));//ist nicht unbedingt das was wir wollen
} //using wiringpi


