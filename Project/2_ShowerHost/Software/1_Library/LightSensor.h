/**
 ******************************************************************************
 * @file    LightSensor.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions prototypes for light sensor.
 *          (For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIGHTSENSOR_H
#define __LIGHTSENSOR_H

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
// Initialize input pin and ADC to sample
void LightSensor_Init(float Threshold);

// Get light state, 1 for environment is dark and light should be on
uint8_t LightSensor_GetState(void);

// Get specific lighting value
float LightSensor_GetValue(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
