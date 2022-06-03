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
#include "Delay.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Status flag
#define COMMUNICATE_OK 0x00
#define COMMUNICATE_ERR 0x01

// Buffer max length
#define COMMUNICATE_MAXLENGTH 1024

// Timeout(ms)
#define Communicate_TimeOut 100

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Information of ZigBee device
uint8_t ZigBee_ID = 0x01;
uint8_t ZigBee_Channel = 15;
uint16_t ZigBee_PANID = 0x1234;
uint16_t ZigBee_GroupID = 0x1111;

// Name and password of WiFi
char WiFi_Name[] = "\"insanity\"";
char WiFi_Password[] = "\"20010120\"";

// IP and port of TCP server
char Socket_IP[] = "\"192.168.137.135\"";
uint16_t Socket_Port = 10000;

// Buffer
uint8_t ZigBee_TXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t ZigBee_RXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t WiFi_TXBuffer[COMMUNICATE_MAXLENGTH];
uint8_t WiFi_RXBuffer[COMMUNICATE_MAXLENGTH];

/* Private function prototypes -----------------------------------------------*/
// ZigBee check acknowledge
uint8_t Communicate_ZigBeeCheck(void);

// WiFi check acknowledge
uint8_t Communicate_WiFiCheck(void);

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize ZigBee and WiFi device.
 * @param  None.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_Init(uint8_t ID, uint8_t Channel, uint16_t PANID, uint16_t GroupID)
{
    ZigBee_ID = ID;
    ZigBee_Channel = Channel;
    ZigBee_PANID = PANID;
    ZigBee_GroupID = GroupID;
    
    Delay_s(1);

    // Initialize ZigBee device
    ZigBee_Init(ZigBee_TXBuffer, ZigBee_RXBuffer);

    // ZigBee recovery
    strcpy(ZigBee_TXBuffer, ZigBee_Recovery);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }
    Delay_s(3);

    // ZigBee config
    if (Communicate_ZigBeeConfig(ZigBee_TypeCoordinator,
                                 ZigBee_DataFormatOnlyData, ZigBee_ID) != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }
    Delay_s(1);

    // Initialize WiFi device
    WiFi_Init(WiFi_TXBuffer, WiFi_RXBuffer);

    // Enter AT command mode
    strcpy(WiFi_TXBuffer, "<->");
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    Communicate_WiFiCheck();
    Delay_s(5);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    Communicate_WiFiCheck();
    Delay_s(1);

    // WiFi recovery
    strcpy(WiFi_TXBuffer, WiFi_Recovery);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }
    Delay_s(1);

    // WiFi config
    if (Communicate_WiFiConfig(WiFi_WiFiModeSTA, WiFi_SocketTCPClient) != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }
    Delay_s(1);

    return COMMUNICATE_OK;
}

/**
 * @brief  Config ZigBee device.
 * @param  DeviceType: Set ZigBee device type, can be coordinator, router and terminal.
 * @param  DataFormat: Format of received data, include head, tail or only data.
 * @param  ID: User set ID for each device.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_ZigBeeConfig(uint8_t DeviceType, uint8_t DataFormat, uint8_t ID)
{
    uint8_t Command[COMMUNICATE_MAXLENGTH];

    // Set device type
    sprintf(Command, "%s%d\r\n", ZigBee_Type, DeviceType);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set channel
    sprintf(Command, "%s%d\r\n", ZigBee_SetChannel, ZigBee_Channel);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set PANID
    sprintf(Command, "%s%04X\r\n", ZigBee_SetPANID, ZigBee_PANID);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set group ID
    sprintf(Command, "%s%04X\r\n", ZigBee_SetGroupID, ZigBee_GroupID);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set ID
    sprintf(Command, "%s%02X%02X\r\n", ZigBee_UserID, DeviceType, ID);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set data format
    sprintf(Command, "%s%d\r\n", ZigBee_DataFormat, DataFormat);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Software reset
    strcpy(ZigBee_TXBuffer, ZigBee_DeviceReset);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }
    Delay_s(3);

    // Status
    strcpy(ZigBee_TXBuffer, "AT+ZIGB_STATUS=?\r\n");
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    return COMMUNICATE_OK;
}

/**
 * @brief  ZigBee transmit data.
 * @param  DeviceType: Device type of destination.
 * @param  ID: ID of destination.
 * @param  Data: Pointer to data buffer.
 * @param  Length: Length of the data.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_ZigBeeTX(uint8_t DeviceType, uint8_t ID, uint8_t *Data, uint16_t Length)
{
    uint8_t Command[COMMUNICATE_MAXLENGTH];

    // Set destination ID
    sprintf(Command, "%s%02X%02X\r\n", ZigBee_Addressing, DeviceType, ID);
    strcpy(ZigBee_TXBuffer, Command);
    ZigBee_TransmitString(strlen(ZigBee_TXBuffer), Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Transmit data
    sprintf(Command, "%s03,%02X%02X,", ZigBee_Send, DeviceType, ID);
    memcpy(Command + 19, Data, Length);
    Command[19 + Length] = '\r';
    Command[20 + Length] = '\n';
    Length += 21;
    memcpy(ZigBee_TXBuffer, Command, Length);
    ZigBee_TransmitString(Length, Communicate_TimeOut);
    if (Communicate_ZigBeeCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    return COMMUNICATE_OK;
}

/**
 * @brief  ZigBee receive data.
 * @param  Data: Pointer to data buffer.
 * @retval Length of received data.
 */
uint16_t Communicate_ZigBeeRX(uint8_t *Data)
{
    uint16_t length; // Received data length

    length = ZigBee_ReceiveString(Communicate_TimeOut);
    if (length == 0)
    {
        return length;
    }

    memcpy(Data, ZigBee_RXBuffer, length);
    return length;
}

/**
 * @brief  ZigBee check acknowledge.
 * @param  None.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_ZigBeeCheck(void)
{
    uint8_t ReceivedData[COMMUNICATE_MAXLENGTH];
    for (int i = 0; i < 5; i++)
    {
        if (Communicate_ZigBeeRX(ReceivedData) == 0)
        {
            continue;
        }
        else if (strstr(ReceivedData, "OK") != NULL)
        {
            return COMMUNICATE_OK;
        }
        Delay_ms(1);
    }
    return COMMUNICATE_ERR;
}

/**
 * @brief  Config WiFi device.
 * @param  WiFiMode: Set WiFi device mode, can be AP or STA.
 * @param  SocketMode: Set socket mode, can be client or server of TCP or UDP.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_WiFiConfig(uint8_t *WiFiMode, uint8_t *SocketMode)
{
    uint8_t Command[COMMUNICATE_MAXLENGTH];

    // Close command echo
    strcpy(WiFi_TXBuffer, WiFi_EchoDisable);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Config netwok
    sprintf(Command, "%s%s,%s,%s,OPEN\r\n", WiFi_WiFiConfig, WiFiMode, WiFi_Name, WiFi_Password);
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Config socket
    sprintf(Command, "%s%s\r\n", WiFi_Socket1EN, WiFi_Disable); // Disable
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    sprintf(Command, "%s%s,%s,%d\r\n", WiFi_Socket1Config, // Config
            WiFi_SocketTCPClient, Socket_IP, Socket_Port);
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    sprintf(Command, "%s%s\r\n", WiFi_Socket1EN, WiFi_Enable); // Enable
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set socket heart
    sprintf(Command, "%s%d,%s\r\n", WiFi_Socket1Heart, 0, "123456");
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Set work mode
    sprintf(Command, "%s%s\r\n", WiFi_WorkMode, WiFi_WorkModeTransparent);
    strcpy(WiFi_TXBuffer, Command);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    if (Communicate_WiFiCheck() != COMMUNICATE_OK)
    {
        return COMMUNICATE_ERR;
    }

    // Reset
    strcpy(WiFi_TXBuffer, WiFi_DeviceReset);
    WiFi_TransmitString(strlen(WiFi_TXBuffer), Communicate_TimeOut);
    Communicate_WiFiCheck();
    Delay_ms(5);

    return COMMUNICATE_OK;
}

/**
 * @brief  WiFi transmit data.
 * @param  Data: Pointer to data buffer.
 * @param  Length: Length of the data.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_WiFiTX(uint8_t *Data, uint16_t Length)
{
    memcpy(WiFi_TXBuffer, Data, Length);
    WiFi_TransmitString(Length, Communicate_TimeOut);
}

/**
 * @brief  WiFi receive data.
 * @param  Data: Pointer to data buffer.
 * @retval Length of received data.
 */
uint16_t Communicate_WiFiRX(uint8_t *Data)
{
    uint16_t length; // Received data length

    length = WiFi_ReceiveString(Communicate_TimeOut);
    if (length == 0)
    {
        return length;
    }

    memcpy(Data, WiFi_RXBuffer, length);
    return length;
}

/**
 * @brief  WiFi check acknowledge.
 * @param  None.
 * @retval Status, COMMUNICATE_OK for success, COMMUNICATE_ERR for not.
 */
uint8_t Communicate_WiFiCheck(void)
{
    uint8_t ReceivedData[COMMUNICATE_MAXLENGTH];
    for (int i = 0; i < 5; i++)
    {
        if (Communicate_WiFiRX(ReceivedData) == 0)
        {
            continue;
        }
        else if (strstr(ReceivedData, "OK") != NULL)
        {
            return COMMUNICATE_OK;
        }
        Delay_ms(1);
    }
    return COMMUNICATE_ERR;
}

/***********************************END OF FILE********************************/
