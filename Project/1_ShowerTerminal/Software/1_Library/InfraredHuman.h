/**
 ******************************************************************************
 * @file    InfraredHuman.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.15
 * @brief   This file contains all the functions prototypes for infrared sensor
 *          for human presence.(For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INFRAREDHUMAN_H
#define __INFRAREDHUMAN_H

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
// Initialize output pin to emit infrared, input pin to receive data
void InfraredHuman_Init(void);

// Get human state, 1 for object presence, 0 for not
uint8_t InfraredHuman_GetState(void);

// Get correct reception number
uint8_t InfraredHuman_GetValue(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
