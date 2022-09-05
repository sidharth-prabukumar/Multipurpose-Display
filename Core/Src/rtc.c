/**
 * @file rtc.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Source file of the RTC interface
 * @date 2022-09-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "rtc.h"

static RTC_HandleTypeDef  hrtc;

App_StatusTypeDef RTC_Init()
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /** Initialize RTC Only
     */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        return APP_ERROR;
    }

    /** Initialize RTC and set the Time and Date
     */
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_OK != HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD))
    {
        return APP_ERROR;
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;

    if (HAL_OK != HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD))
    {
        return APP_ERROR;
    }
    return APP_OK;
}

App_StatusTypeDef RTC_GetDateTime(RTC_TimeTypeDef * pTime, RTC_DateTypeDef * pDate)
{
    if(!pTime || !pDate)
    {
        return APP_ERROR;
    }
    if(HAL_OK != HAL_RTC_GetTime(&hrtc, pTime, RTC_FORMAT_BIN))
    {
        return APP_ERROR;
    }
    if(HAL_OK != HAL_RTC_GetDate(&hrtc, pDate, RTC_FORMAT_BIN))
    {
        return APP_ERROR;
    }
    return APP_OK;
}

App_StatusTypeDef RTC_SetDateTime(RTC_TimeTypeDef * pTime, RTC_DateTypeDef * pDate, uint32_t Format)
{
    if(!pTime || !pDate)
    {
        return APP_ERROR;
    }
    if(Format != RTC_FORMAT_BIN || Format != RTC_FORMAT_BCD)
    {
        return APP_ERROR;
    }

    if (HAL_OK != HAL_RTC_SetTime(&hrtc, pTime, Format))
    {
        return APP_ERROR;
    }

    if (HAL_OK != HAL_RTC_SetDate(&hrtc, pDate, Format))
    {
        return APP_ERROR;
    }
    return APP_OK;
}

char* RTC_GetDayString()
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    if(APP_OK != RTC_GetDateTime(&sTime, &sDate))
    {
        return "error";
    }
	char * weekday[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	return weekday[(sDate.WeekDay)-1];
}
