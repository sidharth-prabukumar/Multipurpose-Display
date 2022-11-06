/**
 * @file main.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Main application header file
 * @date 2022-09-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "stm32f4xx_hal.h"

#define TRUE 1
#define FALSE 0

/**
* @brief App Status structures definition  
* 
*/
typedef enum 
{
  APP_OK       = 0x00U,
  APP_ERROR    = 0x01U
} App_StatusTypeDef;
