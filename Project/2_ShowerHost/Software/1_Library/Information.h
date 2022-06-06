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
// Get each state from host state
#define ShowerHost_Fan 0x10
#define ShowerHost_Light 0x08
#define ShowerHost_WaterSupply 0x04
#define ShowerHost_Repair 0x02
#define ShowerHost_Help 0x01

/* Exported functions --------------------------------------------------------*/
void Information_InitializeRTC(void);
void Information_SetDateTime(RTC_DateTypeDef* Date, RTC_TimeTypeDef* Time);
void Information_GetDateTime(RTC_DateTypeDef* Date, RTC_TimeTypeDef* Time);
uint32_t Information_GetTimeStamp(void);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
