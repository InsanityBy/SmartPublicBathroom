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
// Operation command
#define NFC_CHECK 0x01
#define NFC_SET 0x00

/* Exported functions --------------------------------------------------------*/
void NFC_Init(void);
char NFC_ReadData(uint8_t sector, uint8_t block, uint8_t *keyA, uint8_t *pData);
char NFC_WriteData(uint8_t sector, uint8_t block, uint8_t *keyA, uint8_t *pData);
uint32_t NFC_SetCheckCardNumber(uint32_t CardNumber, uint8_t operation);
uint32_t NFC_SetCheckUserNumber(uint32_t UserNumber, uint8_t operation);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
