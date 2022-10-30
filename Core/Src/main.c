/**
 * @file main.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Main application file
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include "main.h"
#include "rtc.h"
#include "lcd.h"
#include "timer.h"
#include "bmp280.h"
#include "bmp280_types.h"

/* Uncomment the following line to enable UART Debugging */
//#define APP_DEBUG_UART

UART_HandleTypeDef huart1;
SPI_HandleTypeDef hspi3;

static char lcdRow1String[20];
static char lcdRow2String[20];

void SystemClock_Config(void);
static void GPIO_Init(void);
static void USART1_UART_Init(void);
static void SPI3_SPI_Init(void);
static void PrintDateTimeOnLCD(void);
static App_StatusTypeDef GetTimeFromESP32(time_t *);
static void Error_Handler(void);

#ifdef APP_DEBUG_UART
void printmsg(char *format, ...)
{
	char str[80];
	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
}
#endif

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	GPIO_Init();
	Timer_Init();
	USART1_UART_Init();

	Timer_Start();

	// LCD init
	if (APP_OK != LCD_Init())
	{
		Error_Handler();
	}
	LCD_DisplayClear();
	LCD_ReturnHome();

#ifdef APP_DEBUG_UART
	printmsg("RTC on LCD Test...\r\n");
#endif
	
	LCD_PrintString("Synchronizing...");
	HAL_Delay(2500);
    SPI3_SPI_Init();
	time_t time;
	if(APP_OK != GetTimeFromESP32(&time))
	{
		LCD_DisplayClear();
		LCD_ReturnHome();
		LCD_PrintString("Failed sync!");
		Error_Handler();
	}
	struct tm *tp = gmtime(&time);
	if(!tp)
	{
		Error_Handler();
	}

#ifdef APP_DEBUG_UART
	char dayofweek[7][10] = {"Sunday",   "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	printmsg("%02d/%02d/%04d %s %02d:%02d:%02d\r\n", tp->tm_mon+1, tp->tm_mday,
			(1900 + tp->tm_year), dayofweek[tp->tm_wday], tp->tm_hour, tp->tm_min,
			tp->tm_sec);
#endif

	RTC_TimeTypeDef currTime = {0};
	RTC_DateTypeDef currDate = {0};

	currTime.Hours		= (uint8_t)(tp->tm_hour);
	currTime.Minutes 	= (uint8_t)(tp->tm_min);
	currTime.Seconds 	= (uint8_t)(tp->tm_sec);

	currDate.WeekDay 	= (uint8_t)(tp->tm_wday + 1);	/* Weekday in tm goes from 0-6. RTC struct expects 1-7 */
	currDate.Month 		= (uint8_t)(tp->tm_mon + 1);	/* Month in tm goes from 0-11. RTC struct expects 1-12 */
	currDate.Date 		= (uint8_t)(tp->tm_mday);
	currDate.Year 		= (uint8_t)(tp->tm_year - 100); /* (1900 + tp->tm_year - 2000) */

	// RTC Init
	if (APP_OK != RTC_Init(&currTime, &currDate, RTC_FORMAT_BIN))
	{
		Error_Handler();
	}

	/* BMP280 Init */
	if (APP_OK != BMP280_Init())
	{
		Error_Handler();
	}

	/* Set up the BMP280 Config Values */
	bmp280_config_t bmp280_config;
	bmp280_config.filter = BMP280_FILTER_X2;
	bmp280_config.mode = BMP280_MODE_NORMAL;
	bmp280_config.tempOversampling = BMP280_SAMPLING_X1;
	bmp280_config.tStandby = BMP280_STANDBY_MS_500;
	if (APP_OK != BMP280_SetConfig(&bmp280_config))
	{
		Error_Handler();
	}
	/* Confirm that the config values have been set corectly */
	bmp280_config_t bmp280_read_config;
	if (APP_OK != BMP280_GetConfig(&bmp280_read_config))
	{
		Error_Handler();
	}
	if (bmp280_config.filter != bmp280_read_config.filter ||
		bmp280_config.mode != bmp280_read_config.mode ||
		bmp280_config.tempOversampling != bmp280_read_config.tempOversampling ||
		bmp280_config.tStandby != bmp280_read_config.tStandby)
	{
#ifdef APP_DEBUG_UART
		printmsg("BMP280 Config values do not match\r\n");
#endif
		Error_Handler();
	}

#ifdef APP_DEBUG_UART
	printmsg("Day is %s...\r\n", RTC_GetDayString());
	printmsg("Current Time is : %s\r\n", RTC_GetTimeString());
	printmsg("Current Date is (DD-MM-YY): %s\r\n", RTC_GetDateString());
	printmsg("Current Tempature = %s°C\r\n", BMP280_GetTemperatureString());
#endif

	LCD_DisplayClear();
	LCD_ReturnHome();
	LCD_SendCommand(LCD_CMD_DON_CUROFF_BLKOFF);

	/* Infinite loop */
	while (1)
	{
		if (Timer_HasTimerExpired())
		{
			PrintDateTimeOnLCD();
		}
	}
	return 0;
}

/**
 * @brief Configure the system clock
 * This function configures the System Clock 50 MHz using HSE
 * as the clock source
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 50;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	/*Configure the systick timer interrupt frequency (for every 1 ms) */
	uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
	HAL_SYSTICK_Config(hclk_freq / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void SPI3_SPI_Init(void)
{
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_SLAVE;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef usrLED; // PA 5

	usrLED.Pin = GPIO_PIN_5;
	usrLED.Mode = GPIO_MODE_OUTPUT_PP;
	usrLED.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &usrLED);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	__HAL_RCC_GPIOC_CLK_ENABLE();
	usrLED.Pin = GPIO_PIN_13;
	usrLED.Mode = GPIO_MODE_INPUT;
	usrLED.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &usrLED);
}

void PrintDateTimeOnLCD()
{
	RTC_TimeTypeDef currTime = {0};
	RTC_DateTypeDef currDate = {0};
	memset(&currTime, 0, sizeof(currTime));
	memset(&currDate, 0, sizeof(currDate));
	if (APP_OK != RTC_GetDateTime(&currTime, &currDate))
	{
		Error_Handler();
	}

	/* Set Cursor to the beginning */
	LCD_ReturnHome();

	/* Prepare the LCD Strings */
	sprintf(lcdRow1String, "%s %s", RTC_GetTimeString(), RTC_GetDateString());
	sprintf(lcdRow2String, "%s %s%cC    ", RTC_GetDayString(), BMP280_GetTemperatureString(), LCD_DEGREES_CHAR_CODE);

	/* Write to LCD */
	LCD_PrintString(lcdRow1String);
	LCD_SetCursor(2, 1);
	LCD_PrintString(lcdRow2String);
}

static App_StatusTypeDef GetTimeFromESP32(time_t * pTime)
{
#ifdef APP_DEBUG_UART
	printmsg("Waiting for data via SPI...\r\n");
#endif
	char rx_buf[8];
	uint64_t epochSecs = 0;
	uint64_t * pEpochSecs = &epochSecs;
	/**
	 * @brief Wait for time sync...
	 * Ensure that epoch is within the millennium (2022 - 3022) :)
	 */
	while (!((*pEpochSecs > 1640995200) && (*pEpochSecs < 33197904000)))
	{
		memset(rx_buf,0,8);
		if(HAL_OK != HAL_SPI_Receive(&hspi3, (uint8_t *)rx_buf, 8, HAL_MAX_DELAY))
		{
			return APP_ERROR;
		}
		pEpochSecs = (uint64_t *)rx_buf;
#ifdef APP_DEBUG_UART
		printmsg("Received: %x\r\n", (uint32_t)*pEpochSecs);
#endif
		*pTime = (time_t)(*pEpochSecs - 14400);
	}
	return APP_OK;
}

void Error_Handler(void)
{
#ifdef APP_DEBUG_UART
	printmsg("Error!!!\r\n");
#endif
	__disable_irq();
	while (1)
	{
	}
}
