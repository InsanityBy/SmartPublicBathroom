/**
 ******************************************************************************
 * @file    TemperatureHumiditySensor.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions prototypes for temperature - 
 *          humidity sensor.(For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMPERATUREHUMIDITYSENSOR_H
#define __TEMPERATUREHUMIDITYSENSOR_H

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
// Temperature - humidity sensor initialize
uint8_t TemperatureHumiditySensor_Init(float TempThreshold, float HumThreshold);

// Start convert
void Sensor_Convert(void);

// Get temperature
float Sensor_GetTemperature(void);

// Get humidity
float Sensor_GetHumidity(void);

// Get temperature and humidity state, 1 for fan should be on and 0 for off.
uint8_t Sensor_Getstate(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
