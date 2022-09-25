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

static RTC_HandleTypeDef hrtc;

/**
 * @brief Local helper to convert the date and time
 * number to a string.
 * @param num number to convert
 * @param buf Target string to store the conversion
 */
static void RTC_NumberToString(uint8_t num, char *buf);

App_StatusTypeDef RTC_Init(RTC_TimeTypeDef *pTime, RTC_DateTypeDef *pDate, uint32_t Format)
{
	/** Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_OK != HAL_RTC_Init(&hrtc))
	{
		return APP_ERROR;
	}

    return APP_OK;

	/** Initialize RTC and set the Time and Date
	 */
	return RTC_SetDateTime(pTime, pDate, Format);
}

App_StatusTypeDef RTC_GetDateTime(RTC_TimeTypeDef *pTime, RTC_DateTypeDef *pDate)
{
	if (!pTime || !pDate)
	{
		return APP_ERROR;
	}
	if (HAL_OK != HAL_RTC_GetTime(&hrtc, pTime, RTC_FORMAT_BIN))
	{
		return APP_ERROR;
	}
	if (HAL_OK != HAL_RTC_GetDate(&hrtc, pDate, RTC_FORMAT_BIN))
	{
		return APP_ERROR;
	}
	return APP_OK;
}

App_StatusTypeDef RTC_SetDateTime(RTC_TimeTypeDef *pTime, RTC_DateTypeDef *pDate, uint32_t Format)
{
	if (!pTime || !pDate)
	{
		return APP_ERROR;
	}
	if (Format != RTC_FORMAT_BIN && Format != RTC_FORMAT_BCD)
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

char *RTC_GetDayString()
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	if (APP_OK != RTC_GetDateTime(&sTime, &sDate))
	{
		return "error";
	}
	char *weekday[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
	return weekday[(sDate.WeekDay) - 1];
}

char *RTC_GetDateString()
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	if (APP_OK != RTC_GetDateTime(&sTime, &sDate))
	{
		return "error";
	}
	static char buf[9];

	buf[2] = '/';
	buf[5] = '/';

	RTC_NumberToString(sDate.Date, buf);
	RTC_NumberToString(sDate.Month, &buf[3]);
	RTC_NumberToString(sDate.Year, &buf[6]);

	buf[8] = '\0';

	return buf;
}

char *RTC_GetTimeString()
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	if (APP_OK != RTC_GetDateTime(&sTime, &sDate))
	{
		return "error";
	}
	static char buf[9];

	buf[2] = ':';
	buf[5] = ':';

	RTC_NumberToString(sTime.Hours, buf);
	RTC_NumberToString(sTime.Minutes, &buf[3]);
	RTC_NumberToString(sTime.Seconds, &buf[6]);

	buf[8] = '\0';

	return buf;
}

static void RTC_NumberToString(uint8_t num, char *buf)
{
	if (num < 10)
	{
		buf[0] = '0';
		buf[1] = num + 48;
	}
	else if (num >= 10 && num < 99)
	{
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
	}
}
