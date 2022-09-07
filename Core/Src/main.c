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
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "main.h"
#include "rtc.h"
#include "lcd.h"

#define APP_DEBUG_UART

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void GPIO_Init(void);
static void I2C1_Init(void);
static void USART1_UART_Init(void);
static void Error_Handler(void);
static void PrintDateTimeOnLCD(void);

#ifdef APP_DEBUG_UART
void printmsg(char *format,...)
{
  char str[80];
  /*Extract the the argument list using VA apis */
  va_list args;
  va_start(args, format);
  vsprintf(str, format,args);
  HAL_UART_Transmit(&huart1,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
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
  I2C1_Init();
  USART1_UART_Init();

  // LCD init
  if(APP_OK != LCD_Init())
  {
    Error_Handler();
  }
  LCD_DisplayClear();
  LCD_ReturnHome();

#ifdef APP_DEBUG_UART
  printmsg("RTC on LCD Test...\r\n");
#endif

  RTC_TimeTypeDef currTime = {0};
  RTC_DateTypeDef currDate = {0};
  currTime.Hours = 8;
  currTime.Minutes = 50;
  currTime.Seconds = 00;
  currTime.TimeFormat = RTC_HOURFORMAT12_PM;
  
	currDate.WeekDay = RTC_WEEKDAY_TUESDAY;
	currDate.Month = RTC_MONTH_SEPTEMBER;
	currDate.Date = 6;
	currDate.Year = 22;

  // RTC Init
  if(APP_OK != RTC_Init(&currTime, &currDate, RTC_FORMAT_BIN))
  {
    Error_Handler();
  }

#ifdef APP_DEBUG_UART
  printmsg("Day is %s...\r\n", RTC_GetDayString());
  printmsg("Current Time is : %s\r\n", RTC_GetTimeString());
	printmsg("Current Date is (DD-MM-YY): %s\r\n", RTC_GetDateString());
#endif

  LCD_DisplayClear();
  LCD_ReturnHome();

  PrintDateTimeOnLCD();

  /* Infinite loop */
  while (1)
  {

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
	HAL_SYSTICK_Config(hclk_freq/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef usrLED;  //PA 5

	usrLED.Pin = GPIO_PIN_5;
	usrLED.Mode = GPIO_MODE_OUTPUT_PP;
	usrLED.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA,&usrLED);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn,15,0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

void PrintDateTimeOnLCD()
{
  RTC_TimeTypeDef currTime = {0};
  RTC_DateTypeDef currDate = {0};
  memset(&currTime, 0, sizeof(currTime));
  memset(&currDate, 0, sizeof(currDate));
  if(APP_OK != RTC_GetDateTime(&currTime, &currDate))
  {
    Error_Handler();
  }
  char *am_pm;
  am_pm = (currTime.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";

  LCD_PrintString(RTC_GetTimeString());
  LCD_PrintString(am_pm);
  LCD_PrintString(" ");
  LCD_PrintString(RTC_GetDateString());
	LCD_SetCursor(2, 1);
  LCD_PrintString(RTC_GetDayString());
}

void HAL_SYSTICK_Callback()
{/*
  PrintDateTimeOnLCD()*/
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
