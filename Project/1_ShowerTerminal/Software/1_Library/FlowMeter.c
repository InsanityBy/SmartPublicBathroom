/**
  ******************************************************************************
  * @file    FlowMeter.c
  * @author  Ma boyang
  * @version V1.0
  * @date    2022.5.8
  * @brief   This file contains all the functions of the flow meter.(For shower
  *         terminal.)
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FlowMeter.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLOWMETER_CLOCK RCC_AHB1Periph_GPIOA
#define FLOWMETER_PINGROUP GPIOA
#define FLOWMETER_PIN GPIO_Pin_0
#define FLOWMETER_PINSOURCE GPIO_PinSource0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize flow meter and clear value.
 * @param  None.
 * @retval None.
 */
void FlowMeter_Init(void)
{
    // GPIO initialize
    RCC_AHB1PeriphClockCmd(FLOWMETER_CLOCK, ENABLE); // Enable clock
    GPIO_PinAFConfig(FLOWMETER_PINGROUP, FLOWMETER_PINSOURCE, GPIO_AF_TIM2);    //PA0 -> TIM2CH1
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = FLOWMETER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(FLOWMETER_PINGROUP, &GPIO_InitStructure);

    // TIM2 initialize
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Enable clock
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 0xFFFFFFFF;
    TIM_InitStructure.TIM_Prescaler = 1 - 1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

    // Set external clock
    TIM_TIxExternalClockConfig(TIM2, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0x00);

    //Clear TIM2 value
    TIM2->CNT = 0x00000000;
}

/**
 * @brief  Start flow meter.
 * @param  None.
 * @retval None.
 */
void FlowMeter_Start(void)
{
    TIM2->CNT = 0x00000000;
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  Stop flow meter.
 * @param  None.
 * @retval None.
 */
void FlowMeter_Stop(void)
{
    TIM_Cmd(TIM2, DISABLE);
}

/**
 * @brief  Clear flow meter.
 * @param  None.
 * @retval None.
 */
void FlowMeter_Clear(void)
{
    TIM2->CNT = 0x00000000;
}

/**
 * @brief  Get flow meter value(Unit: r).
 * @param  None.
 * @retval Number of the rounds.
 */
uint32_t FlowMeter_GetValueR(void)
{
    return TIM2->CNT;
}

/**
 * @brief  Get flow meter value(Unit: Liter)
 * @param  None.
 * @retval Water consumption in liter.
 */
float FlowMeter_GetValueL(void)
{
    uint32_t number = FlowMeter_GetValueR();
    return (float)number/450;
}

/***********************************END OF FILE********************************/
