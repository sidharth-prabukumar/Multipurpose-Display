/**
 * @file tiemr.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Source file of the timer interface
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "timer.h"

timerLocalData_t timerLocalData;

App_StatusTypeDef Timer_Init()
{
	timerLocalData.htimer6.Instance = TIM6;
	timerLocalData.htimer6.Init.CounterMode = TIM_COUNTERMODE_UP;
	timerLocalData.htimer6.Init.Prescaler = 83;
	timerLocalData.htimer6.Init.Period = 59551 - 1;
	if (HAL_OK != HAL_TIM_Base_Init(&timerLocalData.htimer6))
	{
		return APP_ERROR;
	}
	return APP_OK;
}

App_StatusTypeDef Timer_Start()
{
	if (HAL_OK != HAL_TIM_Base_Start_IT(&timerLocalData.htimer6))
	{
		return APP_ERROR;
	}
	return APP_OK;
}

uint8_t Timer_HasTimerExpired()
{
	if (timerLocalData.hasTimerExpired)
	{
		timerLocalData.hasTimerExpired = FALSE;
		return TRUE;
	}
	return FALSE;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	timerLocalData.hasTimerExpired = TRUE;
}
