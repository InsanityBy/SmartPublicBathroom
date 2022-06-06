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
uint8_t TemperatureHumiditySensor_Init(float TempThreshold, float HumThreshold);
void Sensor_Convert(void);
float Sensor_GetTemperature(void);
float Sensor_GetHumidity(void);
uint8_t Sensor_Getstate(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
