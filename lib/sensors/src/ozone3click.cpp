/****************************************************************************
** Copyright (C) 2020 MikroElektronika d.o.o.
** Contact: https://www.mikroe.com/contact
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
**  USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

/*!
 * @file ozone3.c
 * @brief Ozone 3 Click Driver.
 */
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "ozone3click.h"

#define OZONE3_ADC_RESOLUTION                    4095
#define OZONE3_MPPM_TO_PPM                       1000
#define OZONE3_I2C_BUSY_BIT_MASK                 0x01

// ---------------------------------------------- PRIVATE FUNCTION DECLARATIONS 

/**
 * @brief Enable delay.
 * @details Enable delay for 10 microseconds.
 */
// static void dev_enable_delay ( void );

// ------------------------------------------------ PUBLIC FUNCTION DEFINITIONS


OZONE3CLICK::OZONE3CLICK(uint8_t i2c_address_lmp, uint8_t i2c_address_adc)
    : i2cDevice(i2c_address_lmp), _address(i2c_address_lmp)
{
    address_lmp = i2c_address_lmp;
    address_adc = i2c_address_adc;
}


bool OZONE3CLICK::init()
{
    bool success = setConfig();
    if (success)
        std::cout << "OZONE3CLICK inited" << std::endl;
    return success;
}


bool OZONE3CLICK::setConfig()
{
    setAddress(address_lmp);

    uint8_t config = 0b00000000;
    if (!writeReg(0x01, &config, 1))
    {
        std::cerr << "OZONE3CLICK SET CONFIG FAILED" << std::endl;
        return false;
    }

    config = 0b00000011;
    if (!writeReg(0x10, &config, 1))
    {
        std::cerr << "OZONE3CLICK SET CONFIG FAILED" << std::endl;
        return false;
    }

    config = 0b00100000;
    if (!writeReg(0x11, &config, 1))
    {
        std::cerr << "OZONE3CLICK SET CONFIG FAILED" << std::endl;
        return false;
    }

    config = 0b00000000;
    if (!writeReg(0x12, &config, 1))
    {
        std::cerr << "OZONE3CLICK SET CONFIG FAILED" << std::endl;
        return false;
    }

    return true;
}


    // ozone3_write_byte( ctx, OZONE3_REG_MODECN, OZONE3_DEEP_SLEEP_MODE ); ++
   
    // ozone3_write_byte( ctx, OZONE3_REG_LOCK, OZONE3_WRITE_MODE ); ++
    
    // ozone3_write_byte( ctx, OZONE3_REG_TIACN,  OZONE3_RES_TIA_EXT | 
    //                                            OZONE3_RES_LOAD_100_Ohm ); ++
    
    // ozone3_write_byte( ctx, OZONE3_REG_REFCN, OZONE3_VREF_INT | 
    //                                           OZONE3_PERCENTS_INT_ZERO_50 | 
    //                                           OZONE3_BIAS_POL_NEGATIVE | 
    //                                           OZONE3_PERCENTS_BIAS_0 );

    // return OZONE3_OK;


bool OZONE3CLICK::getOzoneRawValue(int16_t& ozone)
{
    setAddress(address_adc);

    // uint8_t buf[2];
    uint16_t buf;
    if(!readWord(&buf))
    {
        std::cerr << "OZONE3CLICK READ FAILED" << std::endl;
        return false;
    }
    // ozone = (int16_t)((buf[0] << 8) | buf[1]);
    ozone = buf;
    ozone &= OZONE3_ADC_RESOLUTION;

    return true;
}

// uint16_t ozone3_read_adc ( ozone3_t *ctx ) {
//     uint8_t read_data[ 2 ];
//     uint16_t adc_data;
    
//     i2c_master_set_slave_address( &ctx->i2c, OZONE3_ADC_DEVICE_ADDR );
//     i2c_master_read( &ctx->i2c, read_data, 2 );   
    
//     adc_data = read_data[ 0 ];
//     adc_data <<= 8;
//     adc_data |= read_data[ 1 ];
//     adc_data &= OZONE3_ADC_RESOLUTION;
        
//     return adc_data;
// }


bool OZONE3CLICK::getOzone(double& ozone)
{
    int16_t raw;
    if(!getOzoneRawValue(raw))
        return false;
    
    ozone = (double)raw / OZONE3_ADC_RESOLUTION * OZONE3_MPPM_TO_PPM;
    return true;
}

// float ozone3_get_o3_ppm ( ozone3_t *ctx ) {
//     uint16_t adc_data;
//     float temp;
    
//     adc_data = ozone3_read_adc( ctx );
    
//     temp = ( float ) adc_data;
//     temp /= OZONE3_ADC_RESOLUTION;
//     temp *= OZONE3_MPPM_TO_PPM;

//     return temp;
// }
