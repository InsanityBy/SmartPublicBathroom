/**
 ******************************************************************************
 * @file    NFC.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.15
 * @brief   This file contains all the functions prototypes for NFC reader.
 *          (For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFC_H
#define __NFC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize all pins and SPI2
void NFC_PinInit(void);

// Initialize RC522
void NFC_Init(void);

// Get data from card
char NFC_GetData(uint8_t *pData);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
