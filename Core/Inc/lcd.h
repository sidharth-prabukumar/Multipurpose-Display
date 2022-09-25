/**
 * @file lcd.h
 * @author Sidharth (sidharth.prabukumar@gmail.com)
 * @brief Header file of the LCD interface
 * @date 2022-09-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "main.h"

#define LCD_GPIO_PORT_1 GPIOB
#define LCD_GPIO_PORT_2 GPIOC

/* GPIO Port B */
#define LCD_GPIO_RS     GPIO_PIN_3
#define LCD_GPIO_RW     GPIO_PIN_5
#define LCD_GPIO_EN     GPIO_PIN_4
#define LCD_GPIO_D4     GPIO_PIN_10
#define LCD_GPIO_D5     GPIO_PIN_6

/* GPIO Port C */
#define LCD_GPIO_D6     GPIO_PIN_5
#define LCD_GPIO_D7     GPIO_PIN_6

/* LCD Commands */
#define LCD_CMD_4DL_2N_5x8F         0x28
#define LCD_CMD_4DL_2N_5x11F        0x2C
#define LCD_CMD_4DL_1N_5X8F  	    0x20
#define LCD_CMD_4DL_1N_5X11F  	    0x24
#define LCD_CMD_DON_CURON_BLKOFF    0x0E
#define LCD_CMD_DON_CUROFF_BLKOFF   0x0C
#define LCD_CMD_DON_CURON_BLKON     0x0F
#define LCD_CMD_INCADD              0x06 
#define LCD_CMD_DISP_CLR            0x01 
#define LCD_CMD_DISP_RET_HOME       0x02 

/**
 * @brief LCD Initialization Function
 * 
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef LCD_Init(void);

/**
 * @brief Command to send to the LCD
 * 
 * @param command uint8_t command code
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef LCD_SendCommand(uint8_t command);

/**
 * @brief User data to send to the LCD
 * 
 * @param user_data uint8_t user data
 * @return App_StatusTypeDef APP_OK if successful. APP_ERROR otherwise
 */
App_StatusTypeDef LCD_SendData(uint8_t user_data);

/**
 * @brief Clear the LCD Display
 * 
 */
void LCD_DisplayClear(void);

/**
 * @brief Send return home command to the LCD
 * 
 */
void LCD_ReturnHome(void);

/**
 * @brief This function sends a string to the LCD
 * 
 */
void LCD_PrintString(char * message);

/**
 * @brief Set the Cursor in the LCD
 * Set LCD to a specified location given by row and column
 * @param row row number (1, 2, 3, 4)
 * @param column column number (1 to 20) LCD is a 4x20
 */
void LCD_SetCursor(uint8_t row, uint8_t column);
