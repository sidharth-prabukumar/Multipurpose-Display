/**
 * @file bmp280_types.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief BMP280 type and register definitions
 * @date 2022-10-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#define BMP280_I2C_ADDRESS_0 (0x76 << 1) /* I2C Address when SDO is set to ground */
#define BMP280_I2C_ADDRESS_1 (0x77 << 1) /* I2C Address when SDO is set to VCC */

/**
 * @brief BMP280 Register addresses
 */
enum
{
    BMP280_REG_DIG_T1 = 0x88,
    BMP280_REG_DIG_T2 = 0x8A,
    BMP280_REG_DIG_T3 = 0x8C,
    BMP280_REG_CHIPID = 0xD0,
    BMP280_REG_VERSION = 0xD1,
    BMP280_REG_SOFTRESET = 0xE0,
    BMP280_REG_STATUS = 0xF3,
    BMP280_REG_CONTROL = 0xF4,
    BMP280_REG_CONFIG = 0xF5,
    BMP280_REG_TEMPDATA = 0xFA,
};

#define BMP280_CHIPID (0x58)        /* Default Chip ID */

/**
 * @brief Sampling Rate register mask and shift
 */
#define BMP280_SAMPLING_MASK            0xE0
#define BMP280_SAMPLING_SHIFT          (5)
/**
 * @brief Sampling Rate
 */
#define BMP280_SAMPLING_NONE            0x00        /* No over-sampling. */
#define BMP280_SAMPLING_X1              0x01        /* 1x over-sampling. */
#define BMP280_SAMPLING_X2              0x02        /* 2x over-sampling. */
#define BMP280_SAMPLING_X4              0x03        /* 4x over-sampling. */
#define BMP280_SAMPLING_X8              0x04        /* 8x over-sampling. */
#define BMP280_SAMPLING_X16             0x05        /* 16x over-sampling. */

/**
 * @brief Operating Mode register mask and shift
 */
#define BMP280_MODE_MASK              0x03
#define BMP280_MODE_SHIFT            (0)
/** 
 * @brief Operating Mode 
 */
#define BMP280_MODE_SLEEP               0x00    /* Sleep mode. */
#define BMP280_MODE_FORCED              0x01    /* Forced mode. */
#define BMP280_MODE_NORMAL              0x03    /* Normal mode. */
#define BMP280_MODE_SOFT_RESET_CODE     0xB6    /* Software reset. */


/**
 * @brief Filtering level register mask and shift
 */
#define BMP280_FILTER_MASK              0x1C
#define BMP280_FILTER_SHIFT            (2)
/**
 * @brief Filtering level
 */
#define BMP280_FILTER_OFF               0x00    /* No filtering. */
#define BMP280_FILTER_X2                0x01    /* 2x filtering. */
#define BMP280_FILTER_X4                0x02    /* 4x filtering. */
#define BMP280_FILTER_X8                0x03    /* 8x filtering. */
#define BMP280_FILTER_X16               0x04    /* 16x filtering. */

/**
 * @brief Standby duration register mask and shift
 */
#define BMP280_STANDBY_MASK             0xE0
#define BMP280_STANDBY_SHIFT           (5)
/**
 * @brief Standby duration in ms
 */ 
#define BMP280_STANDBY_MS_1             0x00    /* 1 ms standby. */
#define BMP280_STANDBY_MS_63            0x01    /* 62.5 ms standby. */
#define BMP280_STANDBY_MS_125           0x02    /* 125 ms standby. */
#define BMP280_STANDBY_MS_250           0x03    /* 250 ms standby. */
#define BMP280_STANDBY_MS_500           0x04    /* 500 ms standby. */
#define BMP280_STANDBY_MS_1000          0x05    /* 1000 ms standby. */
#define BMP280_STANDBY_MS_2000          0x06    /* 2000 ms standby. */
#define BMP280_STANDBY_MS_4000          0x07    /* 4000 ms standby. */
