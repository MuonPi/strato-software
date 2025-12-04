#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "ads1115.h"



#define ADS1115_DATA_REG        0x00
#define ADS1115_CONFIG_REG      0x01



ADS1115::ADS1115(uint8_t i2c_address)
    : i2cDevice(i2c_address), _address(i2c_address)
{}


bool ADS1115::init()
{
    OS = 0b0;
    MUX = 0b100;
    PGA = 0b010;
    MODE = 0b0;
    DR = 0b100;
    COMP_MODE = 0b0;;
    COMP_POL = 0b0;
    COMP_LAT = 0b0;
    COMP_QUE = 0b11;

    return setConfig();
}


// bool ADS1115::getConfig(uint16_t &config)
// {
//     uint8_t buf[2];
//     if (!readReg(ADS1115_CONFIG_REG, buf, 2))
//     {
//         std::cout << "ADS1115 GET CONFIG FAILED" << std::endl;
//         return false;
//     }
//     else
//     {
//         config = buf[1] | buf[0] >> 8;
//         return true;
//     }
// }


bool ADS1115::setConfig()
{
    uint16_t config =
        OS << 15        |   // Bit 15 No Single Conversion
        MUX << 12       |   // Bit 14:12 AIN0 + (channel << 12)) |    // MUX: AINx gegen GND
        PGA << 9        |   // Bit 11:9  PGA ±2.048 V
        MODE << 8       |   // Bit 8 Continous Mode
        DR << 5         |   // Bit 7:5 Data Rate
        COMP_MODE << 4  |   // Bit 4 Traditional Comparator
        COMP_POL << 3   |   // Bit 3 Comparator Active Low
        COMP_LAT << 2   |   // Bit 2 Nonlatching Comparator
        COMP_QUE;           // Bit 1:0 Comparator disabled
    uint8_t buf[2];
    buf[0] = config >> 8;
    buf[1] = config & 0xFF;

    if (!writeReg(ADS1115_CONFIG_REG, buf, 2))
    {
        std::cout << "ADS1115 SET CONFIG FAILED" << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}


bool ADS1115::setChannel(uint8_t channel)
{
    if(channel > 3)
        return false;
    MUX = 0b100 | channel;
    return setConfig();
}


bool ADS1115::getRawValue(int16_t &value)
{
    uint8_t buf[2];
    if (!readReg(ADS1115_DATA_REG, buf, 2))
    {
        std::cout << "ADS1115 READ FAILED" << std::endl;
        return false;
    }

    value = (int16_t)((buf[0] << 8) | buf[1]);
    return true;
}


bool ADS1115::getVoltage(double &voltage)
{
    int16_t raw;
    double range;
    if (!getRawValue(raw))
        return false;
    
    switch (PGA)
    {
    case 0b000:
        range = 6.144;
        break;
    case 0b001:
        range = 4.096;
        break;
    case 0b010:
        range = 2.048;
        break;
    case 0b011:
        range = 1.024;
        break;
    case 0b100:
        range = 0.512;
        break;
    case 0b101:
        range = 0.256;
        break;
    case 0b110:
        range = 0.256;
        break;
    case 0b111:
        range = 0.256;
        break;
    default:
        return false;
    }

    voltage = raw / 32768.0 * range;
    return true;
}


// /*
// * ADS1115 4ch 16 bit ADC
// */
// constexpr uint16_t HI_RANGE_LIMIT { static_cast<uint16_t>(ADS1115::MAX_ADC_VALUE * 0.8) };
// constexpr uint16_t LO_RANGE_LIMIT { static_cast<uint16_t>(ADS1115::MAX_ADC_VALUE * 0.2) };

// constexpr std::chrono::microseconds loop_delay { 100L };

// bool ADS1115::Sample::operator==(const Sample& other)
// {
//     return (value == other.value && voltage == other.voltage && channel == other.channel && timestamp == other.timestamp);
// }

// bool ADS1115::Sample::operator!=(const Sample& other)
// {
//     return (!(*this == other));
// }

// auto ADS1115::adcToVoltage(int16_t adc, const CFG_PGA pga_setting) -> float
// {
//     return (adc * lsb_voltage(pga_setting));
// }

// ADS1115::ADS1115()
//     : i2cDevice("/dev/i2c-1", 0x48)
// {
//     init();
// }

// ADS1115::ADS1115(uint8_t slaveAddress)
//     : i2cDevice(slaveAddress)
// {
//     init();
// }

// ADS1115::ADS1115(const char* busAddress, uint8_t slaveAddress)
//     : i2cDevice(busAddress, slaveAddress)
// {
//     init();
// }

// ADS1115::ADS1115(const char* busAddress, uint8_t slaveAddress, CFG_PGA pga)
//     : i2cDevice(busAddress, slaveAddress)
// {
//     init();
//     setPga(pga);
// }

// ADS1115::~ADS1115()
// {
// }

// void ADS1115::init()
// {
//     fRate = 0x00; // RATE8
//     fTitle = fName = "ADS1115";
// }

// void ADS1115::setPga(uint8_t channel, CFG_PGA pga)
// {
//     if (channel > 3)
//         return;
//     fPga[channel] = pga;
// }

// void ADS1115::setActiveChannel(uint8_t channel, bool differential_mode)
// {
//     fSelectedChannel = channel;
//     fDiffMode = differential_mode;
// }

// bool ADS1115::setContinuousSampling(bool cont_sampling)
// {
//     fConvMode = (cont_sampling) ? CONV_MODE::CONTINUOUS : CONV_MODE::SINGLE;
//     return writeConfig();
// }

// bool ADS1115::writeConfig(bool startNewConversion)
// {
//     uint16_t conf_reg { 0 };

//     // read in the current contents of config reg only if conv_mode is unknown
//     if (fConvMode == CONV_MODE::UNKNOWN) {
//         if (!readWord(static_cast<uint8_t>(REG::CONFIG), &conf_reg))
//             return false;
//         if ((conf_reg & 0x0100) == 0) {
//             fConvMode = CONV_MODE::CONTINUOUS;
//         } else {
//             fConvMode = CONV_MODE::SINGLE;
//         }
//     }

//     conf_reg = 0;

//     if (fConvMode == CONV_MODE::SINGLE && startNewConversion) {
//         conf_reg = 0x8000; // set OS bit
//     }
//     if (!fDiffMode) {
//         conf_reg |= 0x4000; // single ended mode channels
//     }
//     conf_reg |= (fSelectedChannel & 0x03) << 12; // channel select
//     if (fConvMode == CONV_MODE::SINGLE) {
//         conf_reg |= 0x0100; // single shot mode
//     }
//     conf_reg |= (static_cast<uint8_t>(fPga[fSelectedChannel]) & 0x07) << 9; // PGA gain select

//     // This sets the 8 LSBs of the config register (bits 7-0)
//     conf_reg |= 0x00; // TODO: enable ALERT/RDY pin
//     conf_reg |= (fRate & 0x07) << 5;

//     if (!writeWord(static_cast<uint8_t>(REG::CONFIG), conf_reg))
//         return false;
//     fCurrentChannel = fSelectedChannel;
//     return true;
// }

// void ADS1115::waitConversionFinished(bool& error)
// {
//     uint16_t conf_reg { 0 };
//     // Wait for the conversion to complete, this requires bit 15 to change from 0->1
//     int nloops = 0;
//     while ((conf_reg & 0x8000) == 0 && nloops * fReadWaitDelay / 1000 < 1000) // readBuf[0] contains 8 MSBs of config register, AND with 10000000 to select bit 15
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(fReadWaitDelay));
//         if (!readWord(static_cast<uint8_t>(REG::CONFIG), &conf_reg)) {
//             error = true;
//             return;
//         }
//         nloops++;
//     }
//     if (nloops * fReadWaitDelay / 1000 >= 1000) {
//         if (fDebugLevel > 1)
//             printf("timeout!\n");
//         {
//             error = true;
//             return;
//         }
//     }
//     if (fDebugLevel > 2)
//         printf(" nr of busy adc loops: %d \n", nloops);
//     if (nloops > 1) {
//         fReadWaitDelay += (nloops - 1) * fReadWaitDelay / 10;
//         if (fDebugLevel > 1) {
//             printf(" read wait delay: %6.2f ms\n", fReadWaitDelay / 1000.);
//         }
//     }
//     error = false;
// }

// bool ADS1115::readConversionResult(int16_t& dataword)
// {
//     uint16_t data { 0 };
//     // Read the contents of the conversion register into readBuf
//     if (!readWord(static_cast<uint8_t>(REG::CONVERSION), &data))
//         return false;

//     dataword = static_cast<int16_t>(data);

//     return true;
// }

// ADS1115::Sample ADS1115::getSample(unsigned int channel)
// {
//     //if ( fConvMode != CONV_MODE::SINGLE ) return InvalidSample;
//     std::lock_guard<std::mutex> lock(fMutex);
//     int16_t conv_result; // Stores the 16 bit value of our ADC conversion

//     fConvMode = CONV_MODE::SINGLE;
//     fSelectedChannel = channel;

//     startTimer();

//     // Write the current config to the ADS1115
//     // and begin a single conversion
//     if (!writeConfig(true)) {
//         return InvalidSample;
//     }

//     bool err { false };
//     waitConversionFinished(err);
//     if (err) {
//         return InvalidSample;
//     }

//     if (!readConversionResult(conv_result)) {
//         return InvalidSample;
//     }

//     stopTimer();
//     fLastConvTime = fLastTimeInterval;

//     Sample sample {
//         std::chrono::steady_clock::now(),
//         conv_result,
//         adcToVoltage(conv_result, fPga[fCurrentChannel]),
//         lsb_voltage(fPga[fCurrentChannel]),
//         fCurrentChannel
//     };
//     if (fConvReadyFn && sample != InvalidSample)
//         fConvReadyFn(sample);

//     if (fAGC[fCurrentChannel]) {
//         int eadc = std::abs(conv_result);
//         if (eadc > HI_RANGE_LIMIT && fPga[fCurrentChannel] > PGA6V) {
//             fPga[fCurrentChannel] = CFG_PGA(fPga[fCurrentChannel] - 1);
//             //if (fDebugLevel > 1) printf("ADC input high...setting PGA to level %d\n", fPga[fCurrentChannel]);
//         } else if (eadc < LO_RANGE_LIMIT && fPga[fCurrentChannel] < PGA256MV) {
//             fPga[fCurrentChannel] = CFG_PGA(fPga[fCurrentChannel] + 1);
//             //if (fDebugLevel > 1) printf("ADC input low...setting PGA to level %d\n", fPga[fCurrentChannel]);
//         }
//     }
//     fLastSample[fCurrentChannel] = sample;
//     return sample;
// }

// bool ADS1115::triggerConversion(unsigned int channel)
// {
//     // triggering a conversion makes only sense in single shot mode
//     if (fConvMode == CONV_MODE::SINGLE) {
//         try {
//             std::thread sampler(&ADS1115::getSample, this, channel);
//             sampler.detach();
//             return true;
//         } catch (...) {
//         }
//     }
//     return false;
// }

// ADS1115::Sample ADS1115::conversionFinished()
// {
//     std::lock_guard<std::mutex> lock(fMutex);
//     int16_t conv_result; // Stores the 16 bit value of our ADC conversion

//     if (!readConversionResult(conv_result)) {
//         return InvalidSample;
//     }

//     stopTimer();
//     fLastConvTime = fLastTimeInterval;
//     startTimer();

//     Sample sample {
//         std::chrono::steady_clock::now(),
//         conv_result,
//         adcToVoltage(conv_result, fPga[fCurrentChannel]),
//         lsb_voltage(fPga[fCurrentChannel]),
//         fCurrentChannel
//     };
//     if (fConvReadyFn && sample != InvalidSample)
//         fConvReadyFn(sample);

//     if (fAGC[fCurrentChannel]) {
//         int eadc = std::abs(conv_result);
//         if (eadc > HI_RANGE_LIMIT && fPga[fCurrentChannel] > PGA6V) {
//             fPga[fCurrentChannel] = CFG_PGA(fPga[fCurrentChannel] - 1);
//             //if (fDebugLevel > 1) printf("ADC input high...setting PGA to level %d\n", fPga[fCurrentChannel]);
//         } else if (eadc < LO_RANGE_LIMIT && fPga[fCurrentChannel] < PGA256MV) {
//             fPga[fCurrentChannel] = CFG_PGA(fPga[fCurrentChannel] + 1);
//             //if (fDebugLevel > 1) printf("ADC input low...setting PGA to level %d\n", fPga[fCurrentChannel]);
//         }
//     }
//     fLastSample[fCurrentChannel] = sample;
//     return sample;
// }

// int16_t ADS1115::readADC(unsigned int channel)
// {
//     try {
//         std::future<Sample> sample_future = std::async(&ADS1115::getSample, this, channel);
//         sample_future.wait();
//         if (sample_future.valid()) {
//             Sample sample { sample_future.get() };
//             if (sample != InvalidSample)
//                 return sample.value;
//         }
//     } catch (...) {
//     }
//     return INT16_MIN;
// }

// bool ADS1115::setLowThreshold(int16_t thr)
// {
//     uint8_t writeBuf[3]; // Buffer to store the 3 bytes that we write to the I2C device
//     uint8_t readBuf[2]; // 2 byte buffer to store the data read from the I2C device
//     startTimer();

//     // These three bytes are written to the ADS1115 to set the Lo_thresh register
//     writeBuf[0] = static_cast<uint8_t>(REG::LO_THRESH); // This sets the pointer register to Lo_thresh register
//     writeBuf[1] = (thr & 0xff00) >> 8;
//     writeBuf[2] |= (thr & 0x00ff);

//     // Initialize the buffer used to read data from the ADS1115 to 0
//     readBuf[0] = 0;
//     readBuf[1] = 0;

//     // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
//     // this sets the Lo_thresh register
//     int n = write(writeBuf, 3);
//     if (n != 3)
//         return false;
//     n = read(readBuf, 2); // Read the same register into readBuf for verification
//     if (n != 2)
//         return false;

//     if ((readBuf[0] != writeBuf[1]) || (readBuf[1] != writeBuf[2]))
//         return false;
//     stopTimer();
//     return true;
// }

// bool ADS1115::setHighThreshold(int16_t thr)
// {
//     uint8_t writeBuf[3]; // Buffer to store the 3 bytes that we write to the I2C device
//     uint8_t readBuf[2]; // 2 byte buffer to store the data read from the I2C device
//     startTimer();

//     // These three bytes are written to the ADS1115 to set the Hi_thresh register
//     writeBuf[0] = static_cast<uint8_t>(REG::HI_THRESH); // This sets the pointer register to Hi_thresh register
//     writeBuf[1] = (thr & 0xff00) >> 8;
//     writeBuf[2] |= (thr & 0x00ff);

//     // Initialize the buffer used to read data from the ADS1115 to 0
//     readBuf[0] = 0;
//     readBuf[1] = 0;

//     // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
//     // this sets the Hi_thresh register
//     int n = write(writeBuf, 3);
//     if (n != 3)
//         return false;

//     n = read(readBuf, 2); // Read the same register into readBuf for verification
//     if (n != 2)
//         return false;

//     if ((readBuf[0] != writeBuf[1]) || (readBuf[1] != writeBuf[2]))
//         return false;
//     stopTimer();
//     return true;
// }

// bool ADS1115::setDataReadyPinMode()
// {
//     // c.f. datasheet, par. 9.3.8, p. 19
//     // set MSB of Lo_thresh reg to 0
//     // set MSB of Hi_thresh reg to 1
//     // set COMP_QUE[1:0] to any value other than '11' (default value)
//     bool ok = setLowThreshold(static_cast<int16_t>(0b0000000000000000));
//     ok = ok && setHighThreshold(static_cast<int16_t>(0b1111111111111111));
//     ok = ok && setCompQueue(0x00);
//     return ok;
// }

// bool ADS1115::setCompQueue(uint8_t bitpattern)
// {
//     uint8_t buf[2] { 0, 0 }; // 2 byte buffer to store the data read from the I2C device
//     if (readReg(static_cast<uint8_t>(REG::CONFIG), buf, 2) != 2)
//         return false;
//     buf[1] &= 0b11111100;
//     buf[1] |= bitpattern & 0b00000011;
//     if (writeReg(static_cast<uint8_t>(REG::CONFIG), buf, 2) != 2)
//         return false;
//     return true;
// }

// bool ADS1115::devicePresent()
// {
//     std::uint16_t data {};
//     return readWord(&data);
// }

// bool ADS1115::identify()
// {
//     if (fMode == MODE_FAILED) {
//         return false;
//     }
//     if (!devicePresent()) {
//         return false;
//     }

//     uint16_t dataword { 0 };
//     if (!readWord(static_cast<uint8_t>(REG::CONFIG), &dataword)) {
//         return false;
//     }
//     if (((dataword & 0x8000) == 0) && (dataword & 0x0100)) {
//         return false;
//     }
//     uint16_t dataword2 { 0 };
//     // try to read at addr conf_reg+4 and compare with the previously read config register
//     // both should be identical since only the 2 LSBs of the pointer register are evaluated by the ADS1115
//     if (!readWord(static_cast<uint8_t>(REG::CONFIG) | 0x04, &dataword2)) {
//         return false;
//     }
//     if (dataword != dataword2) {
//         return false;
//     }
//     return true;
// }

// double ADS1115::getVoltage(unsigned int channel)
// {
//     double voltage = 0.;
//     getVoltage(channel, voltage);
//     return voltage;
// }

// void ADS1115::getVoltage(unsigned int channel, double& voltage)
// {
//     int16_t adc = 0;
//     getVoltage(channel, adc, voltage);
// }

// void ADS1115::getVoltage(unsigned int channel, int16_t& adc, double& voltage)
// {
//     Sample sample = getSample(channel);
//     adc = sample.value;
//     voltage = sample.voltage;
// }

// void ADS1115::setAGC(bool state)
// {
//     fAGC[0] = fAGC[1] = fAGC[2] = fAGC[3] = state;
// }

// void ADS1115::setAGC(uint8_t channel, bool state)
// {
//     if (channel > 3)
//         return;
//     fAGC[channel] = state;
// }

// bool ADS1115::getAGC(uint8_t channel) const
// {
//     return fAGC[channel & 0x03];
// }
