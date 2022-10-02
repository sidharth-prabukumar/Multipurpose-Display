/**
 * @file bmp280.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief API header for the BMP280 Temperature Sensor
 * @date 2022-10-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "main.h"


/**
 * @brief Struct to store the config values
 */
typedef struct
{
    uint8_t tStandby;           /* Inactive duration (standby time) in normal mode */
    uint8_t filter;             /* Filter settings */
    uint8_t tempOversampling;   /* Temperature oversampling. */
    uint8_t mode;               /* Device mode */
}bmp280_config_t;

/**
 * @brief Struct to store the temperature calibration values
 */
typedef struct
{
	uint16_t dig_T1;            /* 0x88(LSB)/0x89(MSB) */
	int16_t dig_T2;             /* 0x8A(LSB)/0x8B(MSB) */
	int16_t dig_T3;             /* 0x8C(LSB)/0x8D(MSB) */
}bmp280_temp_calib_t;

/**
 * @brief BMP280 Object
 */
typedef struct
{
    uint8_t i2cAddress;                 /* I2C address of the sensor */
    bmp280_config_t config;             /* Config settings */
    bmp280_temp_calib_t temp_calib;     /* Calibration settings */
    double temperatureC;                /* Temperature in Celsius */
}bmp280_t;

/**
 * @brief BMP280 Initialization
 * 
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef BMP280_Init();

/**
 * @brief Read bmp280_config_t and write it to the passed in pointer
 * 
 * @param config Pointer to bmp280_config_t to be populated
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef BMP280_GetConfig(bmp280_config_t * config);

/**
 * @brief Set BMP280 config values with the one passed in
 * 
 * @param config Pointer to bmp280_config_t to set
 * @return App_StatusTypeDef 
 */
App_StatusTypeDef BMP280_SetConfig(bmp280_config_t * config);

/**
 * @brief Read temperature and return as a Double
 * 
 * @return double Temperature in Celsius
 */
double BMP280_ReadTemperatureC();

/**
 * @brief Read temperature and return the value as a String
 * 
 * @return char* String pointer to the temperature value
 */
char * BMP280_GetTemperatureString();
