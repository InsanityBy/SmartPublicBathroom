/**
 ******************************************************************************
 * @file    Communicata.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.29
 * @brief   This file contains all the functions prototypes for communicate device.
 *          (For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMUNICATE_H
#define __COMMUNICATE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "ZigBee.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize ZigBee device
uint8_t Communicate_Init(uint8_t ID, uint8_t Channel, uint16_t PANID, uint16_t GroupID);

// Config ZigBee device
uint8_t Communicate_ZigBeeConfig(uint8_t DeviceType, uint8_t DataFormat, uint8_t ID);

// ZigBee transmit
uint8_t Communicate_ZigBeeTX(uint8_t DeviceType, uint8_t ID, uint8_t *Data, uint16_t Length);

// ZigBee receive
uint16_t Communicate_ZigBeeRX(uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
