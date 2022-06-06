/**
 ******************************************************************************
 * @file    Audio.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.24
 * @brief   This file contains all the functions prototypes for audio device.
 *          (For shower terminal.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_H
#define __AUDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define Audio_Remind 0x0000
#define Audio_Warn 0x0001
#define Audio_SwipeCard 0x0002
#define Audio_ReserveFirst 0x0003
#define Audio_NoWater 0x0004
#define Audio_ReserveToStop 0x0005
#define Audio_ReserveStopped 0x0006
#define Audio_ObjectLeft 0x0007

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void Audio_Init(void);
void Audio_Play(uint16_t address);
void Audio_PlayAfter(uint16_t address);
void Audio_Loop(void);
void Audio_Stop(void);
void Audio_Volume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
