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
void InfraredObject_Init(uint16_t Threshold);
uint8_t InfraredObject_GetState(void);
uint16_t InfraredObject_GetValue(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
