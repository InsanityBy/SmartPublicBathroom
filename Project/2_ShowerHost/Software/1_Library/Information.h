/**
 ******************************************************************************
 * @file    Information.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.30
 * @brief   This file contains all the functions prototypes for device information.
 *          (For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INFORMAION_H
#define __INFORMAION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
// Shower terminal information typedef
typedef struct
{
    uint8_t HostSerial;          // The serial of the host that terminal belongs to
    uint8_t TerminalSerial;      // The serial of shower terminal device
    uint32_t NFCSerial;          // The number of card number, if in use
    uint32_t UserSerial;         // The number of user, if in use
    RTC_DateTypeDef CurrentDate; // Current date
    RTC_TimeTypeDef CurrentTime; // Current time
    RTC_DateTypeDef ReserveStartTime; // Reserved time start, if reserved
    RTC_DateTypeDef ReserveStopTime; // Reserved time stop, if reserved
    float WaterTemperature;      // The water temperature, if in use
    float WaterFlow;             // The flow of water used, if in use
    float AccountBalance;        // The remain money of account, if in use
    uint8_t TerminalState;       // The state of terminal
                                 // bit 7: supply water or not,
                                 // bit 6: be reserved or not, bit 5: in use or not,
                                 // bit 4: card or object left or not, bit 3: first swipe card or not
                                 // bit 2: take the card off or not, bit 1: need help or not,
                                 // bit 0: need repair or not, bit 15 ~ 9 reserved
    uint8_t StateControl;        // Each bit set 1 indicates that the corresponding state bit is set by the cloud
} ShowerTerminal_InformationTypeDef;

// Shower host information typedef
typedef struct
{
    uint8_t HostSerial;          // The number of shower host device
    RTC_DateTypeDef CurrentDate; // Current date
    RTC_TimeTypeDef CurrentTime; // Current time
    float Temperature;          // The environment temperature
    float Humidity;             // The environment humidity
    float Light;                // The environent brightness
    uint8_t HostState;          // The state of host
                                // bit 4: turn on fan or not, bit 3: turn on light or not,
                                // bit 2: supply water or not, bit 1: need repair or not,
                                // bit 0: need repair or not, bit 7 ~ 5: reserved
    uint8_t StateControl;       // Each bit set 1 indicates that the corresponding state bit is set by the cloud
    uint8_t TerminalNumber;     // Number of terminals connected with host
} ShowerHost_InformationTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// Get each state from terminal state
#define ShowerTerminal_WaterSupply 0x0080
#define ShowerTerminal_Reserve 0x0040
#define ShowerTerminal_InUse 0x0020
#define ShowerTerminal_ObjectLeft 0x0010
#define ShowerTerminal_FirstReadCard 0x0008
#define ShowerTerminal_CardOff 0x0004
#define ShowerTerminal_Help 0x0002
#define ShowerTerminal_Repair 0x0001

// Get each state from host state
#define ShowerHost_Fan 0x10
#define ShowerHost_Light 0x08
#define ShowerHost_WaterSupply 0x04
#define ShowerHost_Repair 0x02
#define ShowerHost_Help 0x01

/* Exported functions --------------------------------------------------------*/
// Initialize RTC
void Information_InitializeRTC(void);

// Set date and time
void Information_SetDateTime(RTC_DateTypeDef* Date, RTC_TimeTypeDef* Time);

// Get date and time
void Information_GetDateTime(RTC_DateTypeDef* Date, RTC_TimeTypeDef* Time);

// Get time stamp in seconds
uint32_t Information_GetTimeStamp(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
