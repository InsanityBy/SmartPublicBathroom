/**
 ******************************************************************************
 * @file    InfraredObject.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions prototypes for infrared sensor
 *          for object presence.(For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INFRAREDOBJECT_H
#define __INFRAREDOBJECT_H

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
// Initialize output pin to emit infrared, input pin and ADC to sample
void InfraredObject_Init(void);

// Get object state, 1 for object presence, 0 for not
uint8_t InfraredObject_GetState(void);

// Get ADC value.
uint16_t InfraredObject_GetValue(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
