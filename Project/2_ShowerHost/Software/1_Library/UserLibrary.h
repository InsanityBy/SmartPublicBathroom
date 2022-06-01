/**
 ******************************************************************************
 * @file    UserLibrary.h
 * @author  Ma Boyang
 * @version V2.0
 * @date    2022.5.30
 * @brief   This file contains all the includes and  functions prototypes for
 *          user libraries.(For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USERLIBRARY_H
#define __USERLIBRARY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "Audio.h"
#include "Button.h"
#include "Communicate.h"
#include "Delay.h"
#include "Display.h"
#include "Information.h"
#include "LightSensor.h"
#include "SignalOutput.h"
#include "TemperatureHumiditySensor.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize all peripherals
void ShowerHost_Init(void);

void ShowerHost_GetWiFiData(void);
void ShowerHost_GetZigBeeData(void);
void ShowerHost_SetDevice(void);

uint8_t ShowerHost_GetRepairState(void);
void ShowerHost_SetRepairState(void);
uint8_t ShowerHost_GetHelpState(void);
void ShowerHost_SetHelpState(void);

void ShowerHost_TemperatureDetect(void);
void ShowerHost_HumidityDetect(void);
void ShowerHost_FanControl(void);

void ShowerHost_LightDetect(void);
void ShowerHost_LightControl(void);

void ShowerHost_DisplayHelp(void);
void ShowerHost_DisplayRepair(void);
void ShowerHost_DisplayNormal(void);

void ShowerHost_SendWiFiData(void);
void ShowerHost_SendZigBeeData(void);
        
#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
