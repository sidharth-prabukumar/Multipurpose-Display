/**
 * @file rtc.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Header file of the RTC interface
 * @date 2022-09-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "main.h"
/**
 * @brief RTC Initialization Function
 * 
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef RTC_Init();

/**
 * @brief RTC Date and Time getter
 * This function reads the date and time from the 
 * RTC and populates pTime and pDate with the values
 * @param pTime Pointer to RTC_TimeTypeDef struct
 * @param pDate Pointer to RTC_DateTypeDef struct
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef RTC_GetDateTime(RTC_TimeTypeDef * pTime, RTC_DateTypeDef * pDate);

/**
 * @brief RTC Date and Time setter
 * This function writes the date and time to the 
 * RTC with the values from pTime and pDate
 * @param pTime Pointer to RTC_TimeTypeDef struct
 * @param pDate Pointer to RTC_DateTypeDef struct
 * @param Format Format of the data. Either RTC_FORMAT_BIN or RTC_FORMAT_BCD
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef RTC_SetDateTime(RTC_TimeTypeDef * pTime, RTC_DateTypeDef * pDate, uint32_t Format);

/**
 * @brief Gets the weekday and returns it as a string
 * 
 * @return char* string of the day of the week
 */
char* RTC_GetDayString();
