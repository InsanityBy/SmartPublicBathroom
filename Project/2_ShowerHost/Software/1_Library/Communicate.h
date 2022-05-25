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
// Initialize ZigBee and WiFi device
uint8_t Communicate_Init(void);

// Config ZigBee device
uint8_t Communicate_ZigBeeConfig(uint8_t DeviceType, uint8_t DataFormat, uint8_t ID);

// ZigBee transmit
uint8_t Communicate_ZigBeeTX(uint8_t ID, uint8_t *Data);

// ZigBee receive
uint8_t Communicate_ZigBeeRX(uint8_t *Data);

// Config WiFi device
uint8_t Communicate_WiFiConfig(uint8_t WorkMode, uint8_t WiFiMode, uint8_t SocketMode);

// WiFi transmit
uint8_t Communicate_WiFiTX(uint8_t *Data);

// WiFi receive
uint8_t Communicate_WiFiRX(uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/