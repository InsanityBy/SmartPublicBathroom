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
void Display_Init(void);
void Display_On(void);
void Display_Off(void);
void Display_Clear(void);
void Display_LightUp(void);
void Display_SetPosition(uint8_t x, uint8_t y);
void Display_ShowChar(uint8_t x, uint8_t y, uint8_t data, uint8_t size);
void Display_ShowString(uint8_t x, uint8_t y, uint8_t *data, uint8_t length, uint8_t size);
void Display_DrawBMP(uint8_t x1, uint8_t y1 ,uint8_t x2, uint8_t y2, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
