#include <thread>
#include <chrono>
#include "i2cdevice.h"
#include "sen0321.h"
// #include "ads1115.h"
// #include "bme280.h"
#include "qmc5883.h"
#include "veml6075.h"




#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// uint16_t x_raw, y_raw, z_raw, temperature_raw;
// double x_real, y_real, z_real, temperature_real;

// uint16_t ozone_raw;

uint16_t uva_raw, uvb_raw;

int main()
{
    // QMC5883 Sensor_QMC5883;
    // Sensor_QMC5883.init();
    // Sensor_QMC5883.readRDYBit();
    // Sensor_QMC5883.getXYZRawValues(x_raw, y_raw, z_raw);
    // std::cout << "XYZ raw: " << x_raw << " " << y_raw << " "  << z_raw << std::endl;
    // Sensor_QMC5883.getXYZMagneticFields(x_real, y_real, z_real);
    // std::cout << "XYZ real: " << x_real << " " << y_real << " "  << z_real << std::endl;
    // Sensor_QMC5883.getTemperatureRawValue(temperature_raw);
    // std::cout << "Temperature raw: " << temperature_raw << std::endl;
    // Sensor_QMC5883.getTemperature(temperature_real);
    // std::cout << "Temperature real: " << temperature_real << std::endl;




    // SEN0321 Sensor_SEN0321;
    // Sensor_SEN0321.init();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // for(int i; i < 100; i++)
    // {
    //     Sensor_SEN0321.getOzonRawValue(ozone_raw);
    //     std::cout << "Ozone raw: " << ozone_raw << std::endl;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }




    VEML6075 Sensor_VEML6075;
    Sensor_VEML6075.init();
    Sensor_VEML6075.getUVRawValue(uva_raw, uvb_raw);
    std::cout << "UV raw: " << uva_raw << " " << uvb_raw << std::endl;
    std::cout << std::endl;




    // unsigned int i2cDevice::fNrDevices = 0;
    // unsigned long int i2cDevice::fGlobalNrBytesRead = 0;
    // unsigned long int i2cDevice::fGlobalNrBytesWritten = 0;
    // std::vector<i2cDevice*> i2cDevice::fGlobalDeviceList;

    // int fHandle = open("/dev/i2c-1", O_RDWR);
    // if (fHandle > 0) {
    //     fNrDevices++;
    //     fGlobalDeviceList.push_back(this);
    // }
    // uint8_t buf[5];
    // uint8_t erg[5];
    // buf[0] = 0x10 & 0b10000000;
    // buf[1] = 0x00;
    // write(buf, 2);
    // read(erg, 2);
    // std::cout << static_cast<int>(erg[0]) << static_cast<int>(erg[1]) << std::endl;

    //  const char *i2cDevice = "/dev/i2c-1";
    //  int file;
    //  __u8 buffer[2];
    //  __u8 reg = 0x0c;

    //  file = open(i2cDevice, O_RDWR);
    //  ioctl(file, I2C_SLAVE, 0x10);
    //  write(file, &reg, sizeof(reg));
    //  read(file, buffer, 2);
    //  close(file);
    //  std::cout << "Gelesene Daten: " << static_cast<int>(buffer[0]) << " " << static_cast<int>(buffer[1]) << std::endl;

}



// #include <iostream>
// #include <linux/i2c-dev.h>
// #include <sys/ioctl.h>
// #include <fcntl.h>
// #include <unistd.h>

// int main() {
//     const char *i2cDevice = "/dev/i2c-1";
//     int file;
//     __u16 reg = 0x00; // Registeradresse, die ausgelesen werden soll
//     __u16 data[20];

//     std::cout << sizeof(data) << std::endl;

//     // Öffnen der I2C-Schnittstelle
//     if ((file = open(i2cDevice, O_RDWR)) < 0) {
//         std::cerr << "Fehler beim Öffnen der I2C-Schnittstelle" << std::endl;
//         return 1;
//     }

//     // Setzen der I2C-Adresse des Sensors
//     if (ioctl(file, I2C_SLAVE, 0x10) < 0) {
//         std::cerr << "Fehler beim Setzen der I2C-Adresse" << std::endl;
//         return 1;
//     }

//     // Schreiben der Registeradresse, von der gelesen werden soll
//     if (write(file, &reg, sizeof(reg)) != sizeof(reg)) {
//         std::cerr << "Fehler beim Schreiben der Registeradresse" << std::endl;
//         return 1;
//     }

//     // Lesen von Daten vom Sensor
//     if (read(file, &data, sizeof(data)) != sizeof(data)) {
//         std::cerr << "Fehler beim Lesen von Daten vom Sensor" << std::endl;
//         return 1;
//     }

//     // Schließen der I2C-Schnittstelle
//     close(file);

//     std::cout << sizeof(data) << std::endl;

//     // Ausgabe der gelesenen Daten
//     int len = sizeof(data);

//     for (int i = 0; i < len; i++){
//         std::cout << i << ": " << std::hex << static_cast<int>(data[i]) << "  ";
//     }
//     std::cout << std::endl;
//     // std::cout << "Daten aus Register 0x0c: " << static_cast<int>(data) << std::endl;

//     return 0;
// }

