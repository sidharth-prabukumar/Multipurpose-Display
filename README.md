# Time and Temperature on LCD

This is a study project implemented to understand the RTC and LCD interfaces through an STM32 based ARM board.

## Goal:

To display Current Date, Time and the ambient temperature on the LCD screen.

## How it Works:
1. Upon bootup, the STM32 and ESP32 boards are initialized simultaneously.
2. After initialization, the ESP32 connects to the given Wi-Fi network.
3. Upon successful connection, the current time in epoch seconds is obtained from a SNTP server.
4. This is trnasmitted via SPI to the STM32 board.
5. Upon successful reception of the time via SPI, the RTC of the STM32 board is initialized with the obtained time and date information.
6. The BMP280 board is initialized via I2C and the current temperature is read periodically.
7. The time/date from the RTC and the temperature from the BMP280 are displayed on the LCD screen.
## High-level Design:
![image](https://user-images.githubusercontent.com/52084290/201549869-12f7b240-5bd5-4425-b8a3-8f1fb4deec5f.png)

## Prototype:
![unnamed](https://user-images.githubusercontent.com/52084290/201550085-ffc3f42f-f98a-49af-83dd-899ac01af1f4.jpg)

