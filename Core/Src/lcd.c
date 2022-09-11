/**
 * @file lcd.c
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Source file of the LDD interface
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "lcd.h"

/**
 * @brief Write the value from data to teh LCD D7,D6,D5,D4 GPIOs
 *
 * @param data Value to write to the data lines of the LCD
 */
static void LCD_WriteDataLines(uint8_t data);

/**
 * @brief Function to enable the LCD
 * for the date write to take place
 */
static void LCD_Enable(void);

/**
 * @brief This function sends a character to the LCD
 *
 */
static void LCD_PrintChar(uint8_t data);

App_StatusTypeDef LCD_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef LcdGPIO;

	LcdGPIO.Pin = LCD_GPIO_RS | LCD_GPIO_RW | LCD_GPIO_EN |
				  LCD_GPIO_D4 | LCD_GPIO_D5 | LCD_GPIO_D6 | LCD_GPIO_D7;
	LcdGPIO.Mode = GPIO_MODE_OUTPUT_PP;
	LcdGPIO.Pull = GPIO_NOPULL;
	LcdGPIO.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_GPIO_PORT, &LcdGPIO);

	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	/* Initialize the LCD for the 4-bit data mode */

	// Add 40 ms delay
	HAL_Delay(40U);

	// RS = 0, RW = 0, D7 = 0, D6 = 0, D5 = 1, D4 = 1
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	LCD_WriteDataLines(0x03);

	// Add 5 ms delay
	HAL_Delay(5U);

	// D7 = 0, D6 = 0, D5 = 1, D4 = 1
	LCD_WriteDataLines(0x03);

	// Add 100 us delay
	HAL_Delay(1U);

	// D7 = 0, D6 = 0, D5 = 1, D4 = 1
	LCD_WriteDataLines(0x03);

	// D7 = 0, D6 = 0, D5 = 1, D4 = 0
	LCD_WriteDataLines(0x02);

	// Function set command
	LCD_SendCommand(LCD_CMD_4DL_2N_5x8F);

	// Display ON and cursor ON
	LCD_SendCommand(LCD_CMD_DON_CURON_BLKOFF);

	// Display Clear
	LCD_DisplayClear();

	// Entry Mode Set
	LCD_SendCommand(LCD_CMD_INCADD);

	return APP_OK;
}

App_StatusTypeDef LCD_SendCommand(uint8_t command)
{
	// Set RS = 0 before sending the command
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	// Set RW = 0 for Write
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	/* Send the command to the LCD */
	// MSB nibble first
	LCD_WriteDataLines(command >> 4);
	// LSB nibble next
	LCD_WriteDataLines(command & 0x0F);
	return APP_OK;
}

App_StatusTypeDef LCD_SendData(uint8_t user_data)
{
	// Set RS = 1 before sending user data
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);
	// Set RW = 0 for Write
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	/* Send the user data to the LCD */
	// MSB nibble first
	LCD_WriteDataLines(user_data >> 4);
	// LSB nibble next
	LCD_WriteDataLines(user_data & 0x0F);
	return APP_OK;
}

void LCD_DisplayClear()
{
	LCD_SendCommand(LCD_CMD_DISP_CLR);
	/* Wait for command execution to complete*/
	HAL_Delay(2U);
}

void LCD_ReturnHome()
{
	LCD_SendCommand(LCD_CMD_DISP_RET_HOME);
	/* Wait for command execution to complete*/
	HAL_Delay(2U);
}

void LCD_PrintString(char *message)
{
	do
	{
		LCD_PrintChar((uint8_t)*message++);
	} while (*message != '\0');
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
	column--;
	switch (row)
	{
	case 1:
		LCD_SendCommand((0x80 + column));
		break;
	case 2:
		LCD_SendCommand((0xC0 + column));
		break;
	case 3:
		LCD_SendCommand((0x94 + column));
		break;
	case 4:
		LCD_SendCommand((0xD4 + column));
		break;
	default:
		break;
	}
}

static void LCD_WriteDataLines(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D7, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D6, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D5, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D4, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	LCD_Enable();
}

static void LCD_Enable()
{
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	HAL_Delay(1);
}

static void LCD_PrintChar(uint8_t data)
{
	// Set RS = 1 before sending user data
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);
	// Set RW = 0 for Write
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	/* Send the char to the LCD */
	// MSB nibble first
	LCD_WriteDataLines(data >> 4);
	// LSB nibble next
	LCD_WriteDataLines(data & 0x0F);
}
