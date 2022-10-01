/**
 * @file bmp280.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Source file of the BMP280 Temperature Sensor
 * @date 2022-10-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bmp280.h"
#include "bmp280_types.h"

I2C_HandleTypeDef hi2c1;

static bmp280_t bmp280;

static void UpdateCalibrationValues();
static App_StatusTypeDef BMP280_I2C1_Init(void);

App_StatusTypeDef BMP280_Init()
{
    uint8_t regVal = 0x00;
    memset(&bmp280, 0, sizeof(bmp280));
    bmp280.i2cAddress = BMP280_I2C_ADDRESS_0;
    if(APP_OK != BMP280_I2C1_Init())
    {
        return APP_ERROR;
    } 

    /* Read Device ID */
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, (uint8_t *)BMP280_REG_CHIPID, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, &regVal, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    /* Check Device ID */
    if (regVal != BMP280_CHIPID)
    {
        return APP_ERROR;
    }

    /* Read Calibration values */
    UpdateCalibrationValues();

    /* Update Config values */
    return BMP280_GetConfig(&bmp280.config);
}

App_StatusTypeDef BMP280_GetConfig(bmp280_config_t *config)
{
    if (!config)
    {
        return APP_ERROR;
    }
    uint8_t regVal = 0x00;

    /* Read Config Register */
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, (uint8_t *)BMP280_REG_CONFIG, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, &regVal, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    config->tStandby = (regVal & BMP280_STANDBY_MASK) >> BMP280_STANDBY_SHIFT;
    config->filter = (regVal & BMP280_FILTER_MASK) >> BMP280_FILTER_SHIFT;

    /* Read Control Register */
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, (uint8_t *)BMP280_REG_CONTROL, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, &regVal, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    config->tempOversampling = (regVal & BMP280_SAMPLING_MASK) >> BMP280_SAMPLING_SHIFT;
    config->mode = (regVal & BMP280_MODE_MASK) >> BMP280_MODE_SHIFT;
    return APP_OK;
}

App_StatusTypeDef BMP280_SetConfig(bmp280_config_t *config)
{
    if (!config)
    {
        return APP_ERROR;
    }
    uint8_t regVal = 0x00;
    uint8_t buf[2];
    /* Read config register */
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, (uint8_t *)BMP280_REG_CONFIG, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, &regVal, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    /* Clear the Standby and Filter values while preserving the other areas */
    regVal &= ~(BMP280_STANDBY_MASK | BMP280_FILTER_MASK);
    /* Update the Standby and Filter register values */
    regVal |= ((config->tStandby << BMP280_STANDBY_SHIFT) & BMP280_STANDBY_MASK);
    regVal |= ((config->filter << BMP280_FILTER_SHIFT) & BMP280_FILTER_MASK);

    /* Write to the Config register with the modified values */
    buf[0] = BMP280_REG_CONFIG;
    buf[1] = regVal;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 2, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }

    /* Read control register */
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, (uint8_t *)BMP280_REG_CONTROL, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, &regVal, 1, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    /* Clear the Sampling and Mode values while preserving the other areas */
    regVal &= ~(BMP280_SAMPLING_MASK | BMP280_MODE_MASK);
    /* Update the Sampling and Mode register values */
    regVal |= ((config->tempOversampling << BMP280_SAMPLING_SHIFT) & BMP280_SAMPLING_MASK);
    regVal |= ((config->mode << BMP280_MODE_SHIFT) & BMP280_MODE_MASK);
    buf[0] = BMP280_REG_CONTROL;
    buf[1] = regVal;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 2, HAL_MAX_DELAY))
    {
        return APP_ERROR;
    }
    return APP_OK;
}

double BMP280_ReadTemperatureC()
{
    uint8_t buf[3];
	int32_t temp_adc = 0;
    double temperatureC;
	int32_t var1 = 0;
	int32_t var2 = 0;
	int32_t t_fine;

    /* Read the temperature ADC registers */
    buf[0] = BMP280_REG_TEMPDATA;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 1, HAL_MAX_DELAY))
    {
        return -1;
    }

    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, buf, 3, HAL_MAX_DELAY))
    {
        return -1;
    }

    /* Format the value read from the ADC */
    temp_adc |= buf[0] << 12;
    temp_adc |= buf[1] << 4;
    buf[2] &= 0x0F;
    temp_adc |= buf[2];

    /* Convert the raw value */
    var1 = ((((temp_adc >> 3) - ((int32_t)bmp280.temp_calib.dig_T1 << 1))) * ((int32_t)bmp280.temp_calib.dig_T2)) >> 11;
    var2 = (((((temp_adc >> 4) - ((int32_t)bmp280.temp_calib.dig_T1)) * ((temp_adc >> 4) - ((int32_t)bmp280.temp_calib.dig_T1))) >> 12) * ((int32_t)bmp280.temp_calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    temperatureC = (t_fine * 5 + 128) >> 8;
    temperatureC /= 100;

    return temperatureC;
}

/**
 * @brief Read and store temperature calibration data
 */
static void UpdateCalibrationValues()
{
    uint8_t buf[2];
    /* Read dig_T1(0x88/0x89) */
    buf[0] = BMP280_REG_DIG_T1;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 1, HAL_MAX_DELAY))
    {
        return;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, buf, 2, HAL_MAX_DELAY))
    {
        return;
    }
    bmp280.temp_calib.dig_T1 = (uint16_t)buf[0] << 8 | (uint16_t)buf[1];

    /* Read dig_T2(0x8A/0x8B) */
    buf[0] = BMP280_REG_DIG_T2;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 1, HAL_MAX_DELAY))
    {
        return;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, buf, 2, HAL_MAX_DELAY))
    {
        return;
    }
    bmp280.temp_calib.dig_T2 = (int16_t)((uint16_t)buf[0] << 8 | (uint16_t)buf[1]);

    /* Read dig_T3(0x8C/0x8D) */
    buf[0] = BMP280_REG_DIG_T3;
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, bmp280.i2cAddress, buf, 1, HAL_MAX_DELAY))
    {
        return;
    }
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, bmp280.i2cAddress, buf, 2, HAL_MAX_DELAY))
    {
        return;
    }
    bmp280.temp_calib.dig_T3 = (int16_t)((uint16_t)buf[0] << 8 | (uint16_t)buf[1]);
}

char * BMP280_GetTemperatureString()
{
    static char buf[20];
    double temperatureC = BMP280_ReadTemperatureC();
    char *tmpSign = (temperatureC < 0) ? "-" : "";
	float tmpVal = (temperatureC < 0) ? -temperatureC : temperatureC;

	int tmpInt1 = tmpVal;                  /* Get the integer */
	float tmpFrac = tmpVal - tmpInt1;      /* Get fraction */
	int tmpInt2 = trunc(tmpFrac * 1000);  /* Turn into integer */

	/* Print as parts, Need 0-padding for fractional bit. */
	sprintf ((char *)buf, "%s%d.%04d", tmpSign, tmpInt1, tmpInt2);
    return buf;
}

/**
 * @brief I2C1 Initialization Function
 */
static App_StatusTypeDef BMP280_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        return APP_ERROR;
    }
    return APP_OK;
}