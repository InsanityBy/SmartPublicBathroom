/**
 ******************************************************************************
 * @file    ZigBee.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.17
 * @brief   This file contains all the functions prototypes for communicata by
 *          ZigBee.(For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ZIGBEE_H
#define __ZIGBEE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// AT Command
#define ZigBee_Recovery "AT+UT_RECOVERY\r\n"
#define ZigBee_DeviceReset "AT+UT_RESET\r\n"

#define ZigBee_Type "AT+ZIGB_TYPE="
#define ZigBee_TypeCoordinator 0x00
#define ZigBee_TypeRouter 0x01
#define ZigBee_TypeTerminal 0x02

#define ZigBee_SetChannel "AT+RF_CHANNEL="
#define ZigBee_SetPANID "AT+ZIGB_PANID="
#define ZigBee_SetGroupID "AT+NET_GROUPID="

#define ZigBee_UserID "AT+UT_UID="

#define ZigBee_Addressing "AT+ZIGB_UIDTOID="

#define ZigBee_DataFormat "AT+ZIGB_RXMODE="
#define ZigBee_DataFormatOnlyData 0x00

#define ZigBee_Send "AT+UT_SEND="

/* Exported functions --------------------------------------------------------*/
// Initialize ZigBee device
void ZigBee_Init(uint8_t *pTransmitData, uint8_t *pReceiveData);

// Transmit 1 byte data
void ZigBee_TransmitByte(uint8_t data);

// Transmit word string
void ZigBee_TransmitString(uint16_t length, uint16_t nTime);

// Receive 1 byte data
uint8_t ZigBee_ReceiveByte(void);

// Receive word string
uint16_t ZigBee_ReceiveString(uint16_t nTime);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
