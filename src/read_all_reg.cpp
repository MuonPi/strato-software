// #include "spidevice.h"
// #include <iostream>
// #include <iomanip>
// #include <lmic.h>
// #include <hal/hal.h>

// void os_getArtEui(u1_t *buf) {}
// void os_getDevEui(u1_t *buf) {}
// void os_getDevKey(u1_t *buf) {}

// // Pin mapping
// const lmic_pinmap lmic_pins =
//     {
//         .nss = 0,
//         .rxtx = 0,
//         .rst = 0,
//         .dio = {0, 0, 0},
//         .rxtx_rx_active = 0,
//         .rssi_cal = 10,
//         .spi_freq = 1000000 /* 1 MHz */
// };

// int main()
// {
//     SPI::spiDevice spi_device{};
//     spi_device.init();
//     for (std::size_t i{0}; i < 0x64; i++)
//     {
//         auto ch = spi_device.read(i, 1)[0];
//         std::cout << i << ": " << std::hex << static_cast<unsigned>(ch) << "\n\n";
//     }
//     std::cout << std::flush;
//     return 0;
// }