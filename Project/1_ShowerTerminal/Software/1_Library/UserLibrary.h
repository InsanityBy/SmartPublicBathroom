/**
 ******************************************************************************
 * @file    UserLibrary.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.6.1
 * @brief   This file contains all the includes and  functions prototypes for
 *          user libraries.(For shower terminal.)
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
#include "FlowMeter.h"
#include "Information.h"
#include "InfraredHuman.h"
#include "InfraredObject.h"
#include "NFC.h"
#include "TemperatureSensor.h"
#include "Valve.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize all peripherals
void ShowerTerminal_Init(void);

uint8_t ShowerTerminal_SendZigBeeData(uint8_t Timeout);
uint8_t ShowerTerminal_GetZigBeeData(uint8_t Timeout);
void ShowerTerminal_SetDevice(void);

uint8_t ShowerTerminal_ObjectLeftCheck(void);
uint8_t ShowerTerminal_GetRepairState(void);
uint8_t ShowerTerminal_GetHelpState(void);
uint8_t ShowerTerminal_HumanDetect(void);
uint8_t ShowerTerminal_ObjectDetect(void);
uint8_t ShowerTerminal_CardDetect(void);
uint8_t ShowerTerminal_CardFirstRead(void);
uint8_t ShowerTerminal_CardTake(void);
uint8_t ShowerTerminal_ReservationCheck(void);

void ShowerTerminal_TakeObjectProcess(void);
void ShowerTerminal_SetRepairState(void);
void ShowerTerminal_SetHelpState(void);
void ShowerTerminal_CardFirstReadProcess(void);
void ShowerTerminal_InUseProcess(void);
void ShowerTerminal_WaterStopRemind(void);
void ShowerTerminal_NearReservationEnd(void);
void ShowerTerminal_NoReservationProcess(void);
void ShowerTerminal_CardTakeProcess(void);
void ShowerTerminal_NotUseProcess(void);
void ShowerTerminal_CardObjectLeft(void);
void ShowerTerminal_IdleState(void);

void ShowerTerminal_DisplayContent(uint8_t *MainContent);
        
#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
