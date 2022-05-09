/**
 ******************************************************************************
 * @file    TemperatureSensor.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.6
 * @brief   This file contains all the functions prototypes for the temperature
 *          sensor library.(For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMPERATURESENSOR_H
#define __TEMPERATURESENSOR_H

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
// Temperature sensor initialize
uint8_t TemperatureSensor_Init(void);

// Start convert
void Sensor_Convert(void);

// Get temperature
float Sensor_GetTemperature(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
