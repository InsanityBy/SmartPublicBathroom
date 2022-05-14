/**
 ******************************************************************************
 * @file    Valve.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.8
 * @brief   This file contains all the functions to control valve.(For shower
 *         terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Valve.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VALVE_CLOCK RCC_AHB1Periph_GPIOA
#define VALVE_PINGROUP GPIOA
#define VALVE_PIN GPIO_Pin_4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Valve state, 1 for on, 0 for off
uint8_t Valve_State = 0x00;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize valve control pin.
 * @param  None.
 * @retval None.
 */
void Valve_Init(void)
{
    RCC_AHB1PeriphClockCmd(VALVE_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = VALVE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(VALVE_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Start valve.
 * @param  None.
 * @retval None.
 */
void Valve_Start(void)
{
    Valve_State = 0x01;
    GPIO_SetBits(VALVE_PINGROUP, VALVE_PIN);
}

/**
 * @brief  Stop valve.
 * @param  None.
 * @retval None.
 */
void Valve_Stop(void)
{
    Valve_State = 0x00;
    GPIO_ResetBits(VALVE_PINGROUP, VALVE_PIN);
}

/**
 * @brief  Get valve state.
 * @param  None.
 * @retval Valve state, 1 for on, 0 for off.
 */
uint8_t Valve_GetState(void)
{
    return Valve_State;
}

/***********************************END OF FILE********************************/
