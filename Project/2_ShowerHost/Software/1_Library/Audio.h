/**
 ******************************************************************************
 * @file    Audio.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.19
 * @brief   This file contains all the functions prototypes for audio device.
 *          (For shower host.)
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
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// Initialize audio device
void Audio_Init(void);

// Play sound, break into current play
void Audio_Play(uint16_t address);

// Play sound after current play
void Audio_PlayAfter(uint16_t address);

// Start loop mode
void Audio_Loop();

// Stop play
void Audio_Stop(void);

// Set volume
void Audio_Volume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
