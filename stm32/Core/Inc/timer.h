/**
 * @file timer.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Header file of the Timer interface
 * @date 2022-09-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "main.h"

typedef struct
{
	TIM_HandleTypeDef htimer6;
	volatile uint8_t hasTimerExpired;
}timerLocalData_t;

/**
 * @brief Initialize the timer peripheral
 * 
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef Timer_Init(void);

/**
 * @brief Start the timer
 * 
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef Timer_Start(void);

/**
 * @brief Reeturn the status of whether the timer has expired or not
 * 
 * @return uint8_t TRUE when timer has expired. FALSE otherwise
 */
uint8_t Timer_HasTimerExpired(void);
