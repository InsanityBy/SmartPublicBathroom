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
#define AudioRemind 0x0000
#define AudioWarn 0x0001
#define AudioHelp 0x0002
#define AudioRepair 0x0003
#define AudioObjectLeft 0x0004
#define AudioTimeUp 0x0005
#define AudioWaterStop 0x0006

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize audio device
void Audio_Init(void);

// Play sound, break into current play
void Audio_Play(uint16_t address);

// Play sound after current play
void Audio_PlayAfter(uint16_t address);

// Start loop mode
void Audio_Loop(void);

// Stop play
void Audio_Stop(void);

// Set volume
void Audio_Volume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
