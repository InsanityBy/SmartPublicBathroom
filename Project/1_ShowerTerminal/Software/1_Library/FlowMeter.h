/**
  ******************************************************************************
  * @file    FlowMeter.h
  * @author  Ma Boyang
  * @version V1.0
  * @date    2022.5.8
  * @brief   This file contains all the functions prototypes for the flow meter 
  *          sensor library.(For shower terminal.)
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
void FlowMeter_Init(void);
void FlowMeter_Start(void);
void FlowMeter_Stop(void);
void FlowMeter_Clear(void);
float FlowMeter_GetValueL(void);
uint32_t FlowMeter_GetValueR(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
