#include "spidevice.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// using namespace SPI;	// NKRG

unsigned int spiDevice::fNrDevices{ 0 };
unsigned long int spiDevice::fGlobalNrBytesWritten{ 0 };
unsigned long int spiDevice::fGlobalNrBytesRead{ 0 };
std::vector<spiDevice*> spiDevice::fGlobalDeviceList;

spiDevice::~spiDevice() {
	if (fHandle > 0) {
		fNrDevices--;
	}
	close(fHandle);
	std::vector<spiDevice*>::iterator it;
	it = std::find(fGlobalDeviceList.begin(), fGlobalDeviceList.end(), this);
	if (it != fGlobalDeviceList.end()) {
		fGlobalDeviceList.erase(it);
	}
}

auto spiDevice::init(std::string busAddress, std::uint32_t speed, Mode mode, uint8_t bits)->bool {
	fNrBits = bits;
	fSpeed = speed;

	// enum class spi_mode -> csyle spi_mode
	switch (mode) {
	case Mode::spi_mode_0:
		fMode = SPI_MODE_0;
		break;
	case Mode::spi_mode_1:
		fMode = SPI_MODE_1;
		break;
	case Mode::spi_mode_2:
		fMode = SPI_MODE_2;
		break;
	case Mode::spi_mode_3:
		fMode = SPI_MODE_3;
		break;
	default:
		fMode = SPI_MODE_0;
		break;
	}

	fHandle = open(busAddress.c_str(), O_RDWR);
	if (fHandle < 0) {
		std::cerr << "Could not open spi device" << std::endl;
		return false;
	}
	fNrDevices++;
	fGlobalDeviceList.push_back(this);

	/*
	 * spi mode
	*/
	int ret{};
	ret = ioctl(fHandle, SPI_IOC_WR_MODE, &fMode);
	if (ret == -1) {
		std::cerr << "can't set spi mode" << std::endl;
		return false;
	}
	ret = ioctl(fHandle, SPI_IOC_RD_MODE, &fMode);
	if (ret == -1) {
		std::cerr << "can't get spi mode" << std::endl;
	}

	/*
	 * bits per word
	*/
	ret = ioctl(fHandle, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		std::cerr << "can't set bits per word" << std::endl;
	}

	ret = ioctl(fHandle, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1) {
		std::cerr << "can't get bits per word" << std::endl;
		return false;
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(fHandle, SPI_IOC_WR_MAX_SPEED_HZ, &fSpeed);
	if (ret == -1) {
		std::cerr << "can't set max speed hz" << std::endl;
	}

	ret = ioctl(fHandle, SPI_IOC_RD_MAX_SPEED_HZ, &fSpeed);
	if (ret == -1) {
		std::cerr << "can't get max speed hz" << std::endl;
	}

	std::cout << "spi initialise successful\n"; //Luisa

	return true;
}

auto spiDevice::devicePresent()->bool {
	return false;
}

auto spiDevice::write(const std::uint8_t command, const std::string& data)->bool {
	if (fHandle==-1) {
		std::cerr << "tried to write to spi without initialising (calling init(..) first)." << std::endl;
		return false;
	}
	const std::size_t n = data.size() + 1;

	auto txBuf{ std::make_unique<std::uint8_t[]>(n) };
	auto rxBuf{ std::make_unique<std::uint8_t[]>(n) };
	txBuf[0] = command;
	for (std::size_t i = 1; i < n; i++) {
		txBuf[i] = data[i - 1];
	}

	auto status = spi_xfer(fHandle, fSpeed, fMode, fNrBits, txBuf.get(), rxBuf.get(), n);
    // std::cout << "write:\ntxbuf: ";
	// for (std::size_t i = 0; i < n ; i++)
	// {
	// 	std::cout << std::hex << static_cast<unsigned>(txBuf[i]) << " ";
	// }
	// std::cout << "\nrxbuf: ";
	// for (std::size_t i = 0; i < n ; i++)
	// {
	// 	std::cout << std::hex << static_cast<unsigned>(rxBuf[i]) << " ";
	// }
	// std::cout << "\n";

	if (status != static_cast<decltype(status)>(n)) {
		std::cerr << "transfer size mismatch: spi_xfer returned " << status << " bytes transfered but should write" << n << " bytes." << std::endl;
		return false;
	}
	return true;
}

auto spiDevice::read(const std::uint8_t command, const std::size_t nBytes)->std::string {
	if (fHandle == -1) {
		std::cerr << "tried to read from spi without initialising." << std::endl;
		return "";
	}
	const std::size_t n = nBytes + 1;

	auto txBuf{ std::make_unique<std::uint8_t[]>(n) };
	auto rxBuf{ std::make_unique<std::uint8_t[]>(n) };
	txBuf[0] = command;

	auto status = spi_xfer(fHandle, fSpeed, fMode, fNrBits, txBuf.get(), rxBuf.get(), n);
    // std::cout << "read:\ntxbuf: ";
	// for (std::size_t i = 0; i < n ; i++)
	// {
	// 	std::cout << std::hex << static_cast<unsigned>(txBuf[i]) << " ";
	// }
	// std::cout << "\nrxbuf: ";
	// for (std::size_t i = 0; i < n ; i++)
	// {
	// 	std::cout << std::hex << static_cast<unsigned>(rxBuf[i]) << " ";
	// }
	// std::cout << "\n";

	if (status != static_cast<decltype(status)>(n)) {
		std::cerr << "transfer size mismatch: spi_xfer returned " << status <<" bytes but should read" << n <<" bytes." << std::endl;
		return "";
	}
	std::string data;
	for (std::size_t i = 1; i < n; i++) {
		data += rxBuf[i];
	}
	return data;
}

auto spiDevice::spi_xfer(const int handle, const uint32_t speed, const uint8_t mode, const uint8_t bits, const uint8_t* tx, uint8_t* rx, uint32_t nBytes)->int {
	int ret{};
	uint16_t delay{};
	spi_ioc_transfer tr = {
		(unsigned long)tx, // tx_buf
		(unsigned long)rx, // rx_buf
		nBytes, // len
		speed, // speed_hz
		delay, // delay_usecs
		bits, // bits_per_word
		0, // cs_change
		bits, // rx_nbits
		bits, // tx_nbits
		mode // mode
		//0 // cs

		/**
		* Note:
		* Depending on spidev version there are different parameters in struct
		*/
	};
	
	tr.tx_nbits = 0; 	// NKRG
	tr.rx_nbits = 0; 	// NKRG

	ret = ioctl(handle, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		std::cerr << "can't send spi message" << std::endl;
	}
	return ret;
}




























// #include "spidevice.h"
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/spi/spidev.h>
// #include <cstring>
// #include <iostream>

// SPIClass SPI;

// bool SPIClass::initDevice(const std::string& dev) {
//     if (fHandle >= 0) return true;
//     fHandle = open(dev.c_str(), O_RDWR);
//     if (fHandle < 0) {
//         std::cerr << "Failed to open SPI device: " << dev << "\n";
//         return false;
//     }
//     return true;
// }

// void SPIClass::begin() {
//     if (!initDevice()) return;
//     // evtl. Default-Einstellungen hier setzen
// }

// void SPIClass::end() {
//     if (fHandle >= 0) {
//         close(fHandle);
//         fHandle = -1;
//     }
// }

// void SPIClass::beginTransaction(SPISettings settings) {
//     fSettings = settings;
//     if (fHandle < 0) return;

//     // SPI-Modus setzen
//     if (ioctl(fHandle, SPI_IOC_WR_MODE, &fSettings.mode) < 0) std::cerr << "Can't set SPI mode\n";
//     if (ioctl(fHandle, SPI_IOC_WR_BITS_PER_WORD, &(uint8_t){8}) < 0) std::cerr << "Can't set bits per word\n";
//     if (ioctl(fHandle, SPI_IOC_WR_MAX_SPEED_HZ, &fSettings.clock) < 0) std::cerr << "Can't set SPI speed\n";
// }

// void SPIClass::endTransaction() {
//     // bei Raspberry Pi gibt es kein echtes Transaction-Konzept
// }

// uint8_t SPIClass::transfer(uint8_t data) {
//     if (fHandle < 0) return 0;
//     uint8_t rx{};
//     struct spi_ioc_transfer tr{};
//     tr.tx_buf = (unsigned long)&data;
//     tr.rx_buf = (unsigned long)&rx;
//     tr.len = 1;
//     tr.speed_hz = fSettings.clock;
//     tr.bits_per_word = 8;
//     tr.delay_usecs = 0;
//     tr.cs_change = 0;

//     if (ioctl(fHandle, SPI_IOC_MESSAGE(1), &tr) < 1) {
//         std::cerr << "SPI transfer failed\n";
//         return 0;
//     }
//     return rx;
// }

// void SPIClass::transfer(uint8_t* buf, size_t len) {
//     if (fHandle < 0 || !buf) return;
//     struct spi_ioc_transfer tr{};
//     tr.tx_buf = (unsigned long)buf;
//     tr.rx_buf = (unsigned long)buf;
//     tr.len = len;
//     tr.speed_hz = fSettings.clock;
//     tr.bits_per_word = 8;
//     tr.delay_usecs = 0;
//     tr.cs_change = 0;

//     if (ioctl(fHandle, SPI_IOC_MESSAGE(1), &tr) < 1) {
//         std::cerr << "SPI transfer failed\n";
//     }
// }