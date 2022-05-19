/**
 ******************************************************************************
 * @file    WiFi.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.19
 * @brief   This file contains all the functions prototypes for communicata by
 *          WiFi.(For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIFI_H
#define __WIFI_H

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
// Initialize WiFi device
void WiFi_Init(uint8_t *pTransmitData, uint8_t *pReceiveData);

// Transmit 1 byte data
void WiFi_TransmitByte(uint8_t data);

// Transmit word string
void WiFi_TransmitString(uint16_t length, uint16_t nTime);

// Receive 1 byte data
uint8_t WiFi_ReceiveByte(void);

// Receive word string
uint16_t WiFi_ReceiveString(uint16_t nTime);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
