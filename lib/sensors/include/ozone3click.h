

#ifndef _OZONE3CLICK_H_
#define _OZONE3CLICK_H_


#include <stdint.h>
#include "i2cdevice.h"



#define OZONE3_REG_STATUS                      0x00
#define OZONE3_REG_LOCK                        0x01
#define OZONE3_REG_TIACN                       0x10
#define OZONE3_REG_REFCN                       0x11
#define OZONE3_REG_MODECN                      0x12

#define OZONE3_WRITE_MODE                      0x00
#define OZONE3_READ_ONLY_MODE                  0x01

#define OZONE3_RES_TIA_EXT                     0x00
#define OZONE3_RES_TIA_2750_Ohm                0x04
#define OZONE3_RES_TIA_3500_Ohm                0x08
#define OZONE3_RES_TIA_7K_Ohm                  0x0C
#define OZONE3_RES_TIA_14K_Ohm                 0x10
#define OZONE3_RES_TIA_35K_Ohm                 0x14
#define OZONE3_RES_TIA_120K_Ohm                0x18
#define OZONE3_RES_TIA_350K_Ohm                0x1C
#define OZONE3_RES_LOAD_10_Ohm                 0x00
#define OZONE3_RES_LOAD_33_Ohm                 0x01
#define OZONE3_RES_LOAD_50_Ohm                 0x02
#define OZONE3_RES_LOAD_100_Ohm                0x03

#define OZONE3_VREF_INT                        0x00
#define OZONE3_VREF_EXT                        0x80
#define OZONE3_PERCENTS_INT_ZERO_20            0x00
#define OZONE3_PERCENTS_INT_ZERO_50            0x20
#define OZONE3_PERCENTS_INT_ZERO_67            0x40
#define OZONE3_INT_ZERO_BYPASSED               0x60
#define OZONE3_BIAS_POL_NEGATIVE               0x00
#define OZONE3_BIAS_POL_POSITIVE               0x10
#define OZONE3_PERCENTS_BIAS_0                 0x00
#define OZONE3_PERCENTS_BIAS_1                 0x01
#define OZONE3_PERCENTS_BIAS_2                 0x02
#define OZONE3_PERCENTS_BIAS_4                 0x03
#define OZONE3_PERCENTS_BIAS_6                 0x04
#define OZONE3_PERCENTS_BIAS_8                 0x05
#define OZONE3_PERCENTS_BIAS_10                0x06
#define OZONE3_PERCENTS_BIAS_12                0x07
#define OZONE3_PERCENTS_BIAS_14                0x08
#define OZONE3_PERCENTS_BIAS_16                0x09
#define OZONE3_PERCENTS_BIAS_18                0x0A
#define OZONE3_PERCENTS_BIAS_20                0x0B
#define OZONE3_PERCENTS_BIAS_22                0x0C
#define OZONE3_PERCENTS_BIAS_24                0x0D
#define OZONE3_FET_DIS                         0x00
#define OZONE3_FET_EN                          0x80
#define OZONE3_DEEP_SLEEP_MODE                 0x00
#define OZONE3_LEAD_MODE_2                     0x01
#define OZONE3_STANDBY_MODE                    0x02
#define OZONE3_LEAD_MODE_3                     0x03
#define OZONE3_TEMP_MODE_TIA_OFF               0x06
#define OZONE3_TEMP_MODE_TIA_ON                0x07
#define OZONE3_DEVICE_ENABLE                   0x00
#define OZONE3_DEVICE_DISABLE                  0x01

#define OZONE3_SET_DEV_ADDR                    0x48
#define OZONE3_ADC_DEVICE_ADDR                 0x4D

// #define OZONE3_MAP_MIKROBUS( cfg, mikrobus )      \
//     cfg.scl = MIKROBUS( mikrobus, MIKROBUS_SCL ); \
//     cfg.sda = MIKROBUS( mikrobus, MIKROBUS_SDA ); \
//     cfg.an = MIKROBUS( mikrobus, MIKROBUS_AN );   \
//     cfg.rst = MIKROBUS( mikrobus, MIKROBUS_RST )

// typedef struct
// {
//     // Output pins
//     digital_out_t  rst;     /**< Enable pin. */
//     // Input pins
//     digital_in_t  an;       /**< Analog pin. */
//     // Modules
//     i2c_master_t i2c;       /**< I2C driver object. */
//     // I2C slave address
//     uint8_t slave_address;  /**< Device slave address (used for I2C driver). */
// } ozone3_t;

// typedef struct
// {
//     pin_name_t  scl;       /**< Clock pin descriptor for I2C driver. */
//     pin_name_t  sda;       /**< Bidirectional data pin descriptor for I2C driver. */
//     pin_name_t  an;        /**< Analog pin. */
//     pin_name_t  rst;       /**< Enable pin. */
//     uint32_t  i2c_speed;   /**< I2C serial speed. */
//     uint8_t   i2c_address; /**< I2C slave address. */
// } ozone3_cfg_t;

// typedef enum
// {
//    OZONE3_OK = 0,
//    OZONE3_ERROR = -1
// } ozone3_return_value_t;



class OZONE3CLICK : public i2cDevice
{
public:
    OZONE3CLICK(uint8_t i2c_address_lmp, uint8_t i2c_address2_adc);

    // void ozone3_cfg_setup ( ozone3_cfg_t *cfg );

    bool init();
    bool setConfig();
    bool getOzoneRawValue(int16_t& ozone);
    bool getOzone(double& ozone);

    /**
     * @brief Ozone 3 default configuration function.
     * @details This function executes a default configuration of Ozone 3
     * Click board.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @return @li @c  0 - Success,
     *         @li @c -1 - Error.
     *
     * See #err_t definition for detailed explanation.
     * @note This function can consist any necessary configuration or setting to put
     * device into operating mode.
     *
     * @endcode
     */
    // err_t ozone3_default_cfg ( ozone3_t *ctx );

    /**
     * @brief Ozone 3 I2C writing function.
     * @details This function writes a desired number of data bytes starting from
     * the selected register by using I2C serial interface.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] reg : Start register address.
     * @param[in] tx_buf : Data to be written.
     * @param[in] tx_len : Number of bytes to be written.
     * @return @li @c  0 - Success,
     *         @li @c -1 - Error.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // err_t ozone3_generic_write ( ozone3_t *ctx, uint8_t reg, uint8_t *tx_buf, uint8_t tx_len );

    /**
     * @brief Ozone 3 I2C reading function.
     * @details This function reads a desired number of data bytes starting from
     * the selected register by using I2C serial interface.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] reg : Start register address.
     * @param[out] rx_buf : Output read data.
     * @param[in] rx_len : Number of bytes to be read.
     * @return @li @c  0 - Success,
     *         @li @c -1 - Error.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // err_t ozone3_generic_read ( ozone3_t *ctx, uint8_t reg, uint8_t *rx_buf, uint8_t rx_len );

    /**
     * @brief Ozone 3 enable function.
     * @details This function enable set state of the RST pin 
     * on the Ozone 3 Click board™. 
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] state : State of the RST.
     * @return @li @c  0 - Success,
     *         @li @c -1 - Error.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // err_t ozone3_enable ( ozone3_t *ctx, uint8_t state );

    /**
     * @brief Ozone 3 write byte function.
     * @details This function writes a byte of data to
     * the selected register by using I2C serial interface 
     * on the Ozone 3 Click board™.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] reg : Register address.
     * @param[in] dev_data : Data to be written.
     * @return @li @c  0 - Success,
     *         @li @c -1 - Error.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // err_t ozone3_write_byte ( ozone3_t *ctx, uint8_t reg, uint8_t dev_data );

    /**
     * @brief Ozone 3 read byte function.
     * @details This function read a byte of data from
     * the selected register by using I2C serial interface 
     * on the Ozone 3 Click board™.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] reg : Register address.
     * @return Output read data.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // uint8_t ozone3_read_byte ( ozone3_t *ctx, uint8_t reg );

    /**
     * @brief Ozone 3 read ADC function.
     * @details This function read the ADC data from
     * by using I2C serial interface on the Ozone 3 Click board™.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @param[in] reg : Register address.
     * @return ADC data.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // uint16_t ozone3_read_adc ( ozone3_t *ctx );

    /**
     * @brief Ozone 3 get O3 ppm function.
     * @details This function get O3 ppm data 
     * by using I2C serial interface on the Ozone 3 Click board™.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @return O3 ppm data.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // float ozone3_get_o3_ppm ( ozone3_t *ctx );

    /**
     * @brief Ozone 3 wait ready function.
     * @details This function wait data ready
     * by using I2C serial interface on the Ozone 3 Click board™.
     * @param[in] ctx : Click context object.
     * See #ozone3_t object definition for detailed explanation.
     * @return O3 ppm data.
     *
     * See #err_t definition for detailed explanation.
     * @note None.
     *
     * @endcode
     */
    // void ozone3_wait_ready ( ozone3_t *ctx );

private:
    uint8_t _address;

    uint8_t address_lmp;
    uint8_t address_adc;
};




// #ifndef _OZONE3CLICK_H_
// #define _OZONE3CLICK_H_


// #include <stdint.h>
// #include "i2cdevice.h"

// class OZONE3CLICK : public i2cDevice
// {
// public:
//     OZONE3CLICK(uint8_t i2c_address);

//     bool init();
//     bool setConfig();
//     bool getOzoneRawValue(int16_t& temperature);
//     bool getOzone(double& temperature);

//     // uint8_t OSR = 0b00;
//     // uint8_t RNG = 0b00;
//     // uint8_t ODR = 0b00;
//     // uint8_t MODE = 0b00;
//     // uint8_t SOFT_RST = 0b0;
//     // uint8_t ROL_PNT = 0b0;
//     // uint8_t INT_ENB = 0b0;

// private:
//     uint8_t _address;
// };



#endif // _OZONE3CLICK_H_