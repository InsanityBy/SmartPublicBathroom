/**
 ******************************************************************************
 * @file    SignalOutput.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.8
 * @brief   This file contains all the functions prototypes to control fan, 
 *          light.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIGNALOUTPUT_H
#define __SIGNALOUTPUT_H

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
// Initialize fan and light control pins
void SignalOutput_Init(void);

// Start fan
void Fan_Start(void);

// Stop fan
void Fan_Stop(void);

//Get fan state
uint8_t Fan_GetState(void);

// Start light
void Light_Start(void);

// Stop light
void Light_Stop(void);

//Get light state
uint8_t Light_GetState(void);

#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE*****************************/
