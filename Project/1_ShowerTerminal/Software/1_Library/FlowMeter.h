/**
  ******************************************************************************
  * @file    FlowMeter.h
  * @author  Ma Boyang
  * @version V1.0
  * @date    2022.5.8
  * @brief   This file contains all the functions prototypes for the flow meter 
  *          sensor library.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLOWMETER_H
#define __FLOWMETER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/  
//Flow meter initialize 
void FlowMeter_Init(void);

//Start flow meter
void FlowMeter_Start(void);

//Stop flow meter
void FlowMeter_Stop(void);

//Clear flow meter
void FlowMeter_Clear(void);

//Get flow meter value(Unit: Liter)
float FlowMeter_GetValueL(void);

//Get flow meter value(Unit: r)
uint32_t FlowMeter_GetValueR(void);

#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE*****************************/
