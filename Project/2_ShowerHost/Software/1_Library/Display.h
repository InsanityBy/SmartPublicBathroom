/**
 ******************************************************************************
 * @file    Display.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.20
 * @brief   This file contains all the functions prototypes for display device.
 *          (For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H
#define __DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// Set font size
#define FONTSIZE_16 0x01
#define FONTSIZE_8 0x00

/* Exported functions --------------------------------------------------------*/
// Initialize display device
void Display_Init(void);

// Turn on display
void Display_On(void);

// Turn off display
void Display_Off(void);

// Clear screen
void Display_Clear(void);

// Turn on and light up screen
void Display_LightUp(void);

// Set position
void Display_SetPosition(uint8_t x, uint8_t y);

// Show char on screen
void Display_ShowChar(uint8_t x, uint8_t y, uint8_t data, uint8_t size);

// Show string on screen
void Display_ShowString(uint8_t x, uint8_t y, uint8_t *data, uint8_t length, uint8_t size);

// Draw a bmp image on screen
void Display_DrawBMP(uint8_t x1, uint8_t y1 ,uint8_t x2, uint8_t y2, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
