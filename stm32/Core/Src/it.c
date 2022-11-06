/**
 * @file it.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Interrupt handlers go here
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "main.h"
#include "timer.h"

extern timerLocalData_t timerLocalData;

/**
 * @brief This function handles System tick timer.
 *
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&timerLocalData.htimer6);
}
