/**
 ******************************************************************************
 * @file    Valve.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.8
 * @brief   This file contains all the functions prototypes to control valve.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VALVE_H
#define __VALVE_H

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
// Initialize valve control pin
void Valve_Init(void);

// Start valve
void Valve_Start(void);

// Stop valve
void Valve_Stop(void);

//Get valve state
uint8_t Valve_GetState(void);

#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE*****************************/
