/**
 ******************************************************************************
 * @file    Delay.h
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.7
 * @brief   This file contains all the functions prototypes for delay using
 *          systick.(For shower host.)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H
#define __DELAY_H

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
void Delay_us(uint32_t n);
void Delay_ms(uint32_t n);
void Delay_s(uint32_t n);

#ifdef __cplusplus
}
#endif

#endif

/***********************************END OF FILE********************************/
