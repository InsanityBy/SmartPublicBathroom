/**
 ******************************************************************************
 * @file    Button.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.8
 * @brief   This file contains all the functions for button. To do something
 *         when button pushed, add functions between FUNCTION BEGIN and FUNCTION
 *         END.(For shower terminal.)
 * @note    Follow steps to use.
 *          - Only need to use Button_Init() to initialize.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "UserLibrary.h"
#include "Button.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Repair button - PA5
#define REPAIR_CLOCK RCC_AHB1Periph_GPIOA
#define REPAIR_PINGROUP GPIOA
#define REPAIR_PIN GPIO_Pin_5
#define REPAIR_EXTIPINGROUP EXTI_PortSourceGPIOA
#define REPAIR_EXTIPINSOURCE EXTI_PinSource5
#define REPAIR_EXTILINE EXTI_Line5

// Help button - PA6
#define HELP_CLOCK RCC_AHB1Periph_GPIOA
#define HELP_PINGROUP GPIOA
#define HELP_PIN GPIO_Pin_6
#define HELP_EXTIPINGROUP EXTI_PortSourceGPIOA
#define HELP_EXTIPINSOURCE EXTI_PinSource6
#define HELP_EXTILINE EXTI_Line6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize TIM3 for repair button debounce.
 * @param  None.
 * @retval None.
 */
void TIM3_Init(void)
{
    // Timer initialize, count period 10ms
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 100 - 1;
    TIM_InitStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseInit(TIM3, &TIM_InitStructure);

    // NVIC config for TIM4
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

/**
 * @brief  Initialize TIM4 for help button debounce.
 * @param  None.
 * @retval None.
 */
void TIM4_Init(void)
{
    // Timer initialize, count period 10ms
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 100 - 1;
    TIM_InitStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseInit(TIM4, &TIM_InitStructure);

    // NVIC config for TIM4
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

/**
 * @brief  Initialize button input pins.
 * @param  None.
 * @retval None.
 */
void Button_Init(void)
{
    // Repair button pin initialize
    RCC_AHB1PeriphClockCmd(REPAIR_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = REPAIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(REPAIR_PINGROUP, &GPIO_InitStructure);

    // EXTI config for repair pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    EXTI_InitTypeDef EXTI_InitStructure;
    SYSCFG_EXTILineConfig(REPAIR_EXTIPINGROUP, REPAIR_EXTIPINSOURCE);
    EXTI_InitStructure.EXTI_Line = REPAIR_EXTILINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Help button pin initialize
    RCC_AHB1PeriphClockCmd(HELP_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = HELP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(HELP_PINGROUP, &GPIO_InitStructure);

    // EXTI config for help pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(HELP_EXTIPINGROUP, HELP_EXTIPINSOURCE);
    EXTI_InitStructure.EXTI_Line = HELP_EXTILINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC config
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Debounce timer initialize
    TIM3_Init();
    TIM4_Init();
}

/**
 * @brief  Button interrupt function.
 * @param  None.
 * @retval None.
 */
void EXTI9_5_IRQHandler(void) // When button pushed, start timer wait 10ms and check again
{
    // Repair button
    if (EXTI_GetITStatus(REPAIR_EXTILINE) != RESET)
    {
        EXTI_ClearITPendingBit(REPAIR_EXTILINE);
        TIM_Cmd(TIM3, ENABLE);
    }

    // Help button
    if (EXTI_GetITStatus(HELP_EXTILINE) != RESET)
    {
        EXTI_ClearITPendingBit(HELP_EXTILINE);
        TIM_Cmd(TIM4, ENABLE);
    }
}

/**
 * @brief  TIM3 interrupt function.
 * @param  None.
 * @retval None.
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        // Clear flag, stop timer and clear count value
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        TIM_Cmd(TIM3, DISABLE);
        TIM_SetCounter(TIM3, 0x00000000);

        // Check again
        if (!GPIO_ReadInputDataBit(REPAIR_PINGROUP, REPAIR_PIN))
        {
            /***************************FUNCTION BEGIN****************************/
            uint8_t data[256];
            sprintf(data, "Repair\n");
            Display_ShowString(0, 0, data, 0xFF, FONTSIZE_16);
            Delay_ms(1000);
            Display_Clear();

            /***************************FUNCTION END******************************/
        }
    }
}

/**
 * @brief  TIM4 interrupt function.
 * @param  None.
 * @retval None.
 */
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        // Clear flag, stop timer and clear count value
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        TIM_Cmd(TIM4, DISABLE);
        TIM_SetCounter(TIM4, 0x00000000);

        // Check again
        if (!GPIO_ReadInputDataBit(HELP_PINGROUP, HELP_PIN))
        {
            /***************************FUNCTION BEGIN****************************/
            uint8_t data[256];
            sprintf(data, "Help\n");
            Display_ShowString(0, 2, data, 0xFF, FONTSIZE_16);
            Delay_ms(1000);
            Display_Clear();

            /***************************FUNCTION END******************************/
        }
    }
}

/***********************************END OF FILE********************************/
