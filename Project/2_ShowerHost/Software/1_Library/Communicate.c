/**
 ******************************************************************************
 * @file    Communicata.c
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.23
 * @brief   This file contains all the functions for communicate device.
 *          (For shower host.)
 * @note    Follow steps to use.
 *          - Use Communicate_Init() to initialize device.
 *          - Use Communicate_ZigBeeConfig(uint8_t DeviceType, uint8_t DataFormat)
 *              to set the device type to coordinator, router or terminal; set
 *              the data format, like only data, head + data. NOTE: Set device ID
 *              by writing 1 byte ID to ZigBee_ID variable.
 *          - Use Communicate_ZigBeeTX(uint8_t ID, uint8_t *Data) to transmit data
 *              to ID device.
 *          - Use Communicate_ZigBeeRX(uint8_t *Data) to get the received data.
 *          - Use Communicate_WiFiConfig(uint8_t WorkMode, uint8_t WiFiMode, uint8_t
 *              SocketMode) to set the device work mode to AT command or transparent
 *              transmission; set the WiFi mode to STA or AP; set the socket mode
 *              to server or client using TCP or UDP. NOTE: Set WiFi name and password
 *              by writing to WiFi_Name and WiFi_Password variable.
 *          - Use Communicate_WiFiTX(uint8_t *Data) to transmit data to ID device.
 *          - Use Communicate_WiFiRX(uint8_t *Data) to get the received data.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Communicate.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Status flag
#define COMMUNICATE_OK 0x00
#define COMMUNICATE_ERR 0x01

// Buffer max length
#define COMMUNICATE_MAXLENGTH 256

// Time out
#define Communicate_TimeOut 500

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// ID of ZigBee device
uint8_t ZigBee_ID = 0x01;

// Name and password of WiFi
char WiFi_Name[] = "insanity";
char WiFi_Password[] = "20010120";

// Buffer
uint8_t ZigBee_TXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t ZigBee_RXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t WiFi_TXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t WiFi_RXBuffer[COMMUNICATE_MAXLENGTH];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize ZigBee and WiFi device.
 * @param  None.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_Init(void)
{
    // Initialize ZigBee device
    ZigBee_Init(ZigBee_TXBuffer, ZigBee_RXBuffer);
    strcpy(ZigBee_TXBuffer, ZigBee_DeviceReset); // Software reset
    ZigBee_TransmitString(sizeof(ZigBee_RXBuffer) / sizeof(ZigBee_RXBuffer[0]) - 1,
                          Communicate_TimeOut);
        
}

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

/***********************************END OF FILE********************************/