/**
 ******************************************************************************
 * @file    Audio.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.17
 * @brief   This file contains all the functions for audio device.(For shower
 *          host.)
 * @note    Follow steps to use.
 *          - Use Audio_Init() to initialize audio device.
 *          - Use Audio_Play(uint16_t address) to play sound. NOTE: WILL BREAK
 *              CURRENT PLAY!
 *          - Use Audio_PlayAfter(uint16_t address) to play sound when finished.
 *          - Use Audio_Stop(void) to stop play.
 *          - Use Audio_Volume(uint8_t volume) to set volume.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "WiFi.h"
#include "Delay.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Audio data pin - PB3
#define AUDIODATA_CLOCK RCC_AHB1Periph_GPIOB
#define AUDIODATA_PINGROUP GPIOB
#define AUDIODATA_PIN GPIO_Pin_3

// Audio clovk pin - PB10
#define AUDIOCLOCK_CLOCK RCC_AHB1Periph_GPIOB
#define AUDIOCLOCK_PINGROUP GPIOB
#define AUDIOCLOCK_PIN GPIO_Pin_10

// GPIO set and reset function
#define AUDIODATA_HIGH GPIO_SetBits(AUDIODATA_PINGROUP, AUDIODATA_PIN)
#define AUDIODATA_LOW GPIO_ResetBits(AUDIODATA_PINGROUP, AUDIODATA_PIN)
#define AUDIOCLOCK_HIGH GPIO_SetBits(AUDIOCLOCK_PINGROUP, AUDIOCLOCK_PIN)
#define AUDIOCLOCK_LOW GPIO_ResetBits(AUDIOCLOCK_PINGROUP, AUDIOCLOCK_PIN)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize GPIO pins.
 * @param  pTransmitData: Pointer of transmit data buffer.
 * @param  pReceiveData: Pointer of receive data buffer.
 * @retval None.
 */
void Audio_Init(void)
{
    // GPIO initialize
    GPIO_InitTypeDef GPIO_InitStructure;
    // Audio data pin - PB3
    RCC_AHB1PeriphClockCmd(AUDIODATA_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = AUDIODATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(AUDIODATA_PINGROUP, &GPIO_InitStructure);

    // Audio clovk pin - PB10
    RCC_AHB1PeriphClockCmd(AUDIOCLOCK_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = AUDIOCLOCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(AUDIOCLOCK_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Transmit 1 byte data.
 * @param  data: 1 byte data to transmit.
 * @retval None.
 */
void Audio_TransmitByte(uint8_t data)
{
    // Start bit
    AUDIOCLOCK_LOW;
    Delay_ms(5);

    // Data
    for (int i = 0; i < 8; i++)
    {
        AUDIOCLOCK_LOW;  // Clock low, change data
        if (data & 0x01) // Send "1"
        {
            AUDIODATA_HIGH;
        }
        else // Send "0"
        {
            AUDIODATA_LOW;
        }
        Delay_us(200);
        AUDIOCLOCK_HIGH; // Clock high, sample data
        Delay_us(200);
        data = data >> 1;
    }

    // Idle
    AUDIOCLOCK_HIGH;
    AUDIODATA_HIGH;
}

/**
 * @brief  Transmit 16 bit data.
 * @param  data: 16 bit data to transmit.
 * @retval None.
 */
void Audio_Transmit16Bits(uint16_t data)
{
    uint8_t high, low;
    low = data & 0x00FF;
    high = data >> 8;
    Audio_TransmitByte(high);
    Delay_ms(2);
    Audio_TransmitByte(low);
    Delay_ms(2);
}

/**
 * @brief  Play sound, break into current play.
 * @param  address: Address of sound, from 0x0000 to 0x03E7.
 * @retval None.
 */
void Audio_Play(uint16_t address)
{
    Audio_Transmit16Bits(address);
}

/**
 * @brief  Play sound after current play.
 * @param  address: Address of sound, from 0x0000 to 0x03E7.
 * @retval None.
 */
void Audio_PlayAfter(uint16_t address)
{
    Audio_Transmit16Bits(0xFFF3);
    Audio_Transmit16Bits(address);
}

/**
 * @brief  Start loop mode. When in loop, this function will stop loop mode.
 * @param  None.
 * @retval None.
 */
void Audio_Loop(void)
{
    Audio_Transmit16Bits(0xFFF2);
}

/**
 * @brief  Stop play.
 * @param  None.
 * @retval None.
 */
void Audio_Stop(void)
{
    Audio_Transmit16Bits(0xFFFE);
}

/**
 * @brief  Set volume.
 * @param  volume: The level of volume, from 0 to 15.
 * @retval None.
 */
void Audio_Volume(uint8_t volume)
{
    Audio_Transmit16Bits(0xFFE0 + volume);
}

/***********************************END OF FILE********************************/
