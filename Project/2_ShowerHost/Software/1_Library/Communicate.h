/**
 ******************************************************************************
 * @file    Communicata.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.21
 * @brief   This file contains all the functions prototypes for communicate device.
 *          (For shower host.)
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
#include "WiFi.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
uint8_t Communicate_Init(uint8_t ID, uint8_t Channel, uint16_t PANID, uint16_t GroupID);
uint8_t Communicate_ZigBeeConfig(uint8_t DeviceType, uint8_t DataFormat, uint8_t ID);
uint8_t Communicate_ZigBeeTX(uint8_t DeviceType, uint8_t ID, uint8_t *Data, uint16_t Length);
uint16_t Communicate_ZigBeeRX(uint8_t *Data);
uint8_t Communicate_WiFiConfig(uint8_t *WiFiMode, uint8_t *SocketMode);
uint8_t Communicate_WiFiTX(uint8_t *Data, uint16_t Length);
uint16_t Communicate_WiFiRX(uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
