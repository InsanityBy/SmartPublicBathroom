/**
 ******************************************************************************
 * @file    InfraredObject.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions for infrared object sensor.(For
 *          shower terminal.)
 * @note    Follow steps to use.
 *          - Use InfraredObject_Init() to initialize device.
 *          - Use InfraredObject_GetState() to get object presence state.
 *          - Use InfraredObject_GetValue() to get specific ADC sample value.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "InfraredObject.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Infrared objeect sensor receive pin - PC2
#define INFRAREDOBJECTRX_CLOCK RCC_AHB1Periph_GPIOC
#define INFRAREDOBJECTRX_PINGROUP GPIOC
#define INFRAREDOBJECTRX_PIN GPIO_Pin_2

// Infrared objeect sensor transmit pin - PA3
#define INFRAREDOBJECTTX_CLOCK RCC_AHB1Periph_GPIOA
#define INFRAREDOBJECTTX_PINGROUP GPIOA
#define INFRAREDOBJECTTX_PIN GPIO_Pin_3

// Use ADC1, Channel 12
#define ADC_CHANNEL ADC_Channel_12

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Infrared intensity threshold indicating object presence
uint16_t Threshold_Object = 2048;

// Sample 10 times and get average value
uint16_t Data_Object[10];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize input pin and ADC to sample.
 * @param  None.
 * @retval None.
 */
void InfraredObjectRX_Init(void)
{
    // GPIO config
    RCC_AHB1PeriphClockCmd(INFRAREDOBJECTRX_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INFRAREDOBJECTRX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(INFRAREDOBJECTRX_PINGROUP, &GPIO_InitStructure);

    // ADC common initialize
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_DeInit();
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_15Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    // ADC1 initialize
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Channel config
    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_15Cycles);

    // Interrupt config
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

/**
 * @brief  Initialize output pin to emit infrared.
 * @param  None.
 * @retval None.
 */
void InfraredObjectTX_Init(void)
{
    RCC_AHB1PeriphClockCmd(INFRAREDOBJECTTX_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INFRAREDOBJECTTX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(INFRAREDOBJECTTX_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Initialize input and output pin.
 * @param  None.
 * @retval None.
 */
void InfraredObject_Init(void)
{
    InfraredObjectRX_Init();
    InfraredObjectTX_Init();
}

/**
 * @brief  Get object state.
 * @param  None.
 * @retval 1 for object presence, 0 for not.
 */
uint8_t InfraredObject_GetState(void)
{
    if (InfraredObject_GetValue() > Threshold_Object)
        return 0;
    else
        return 1;
}

/**
 * @brief  Get ADC sample value.
 * @param  None.
 * @retval ADC sample value of infrared sensor.
 */
uint16_t InfraredObject_GetValue(void)
{
    uint16_t Intensity = 0;

    // Enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // Emit infrared
    GPIO_SetBits(INFRAREDOBJECTTX_PINGROUP, INFRAREDOBJECTTX_PIN);

    // Receive and sample
    for (int i = 0; i < 10; i++)
    {
        ADC_SoftwareStartConv(ADC1);
        Delay_us(1);
    }
    for (int i = 0; i < 10; i++)
    {
        Intensity += Data_Object[i];
    }
    Intensity /= 10;

    // Stop emitting to save power
    GPIO_ResetBits(INFRAREDOBJECTTX_PINGROUP, INFRAREDOBJECTTX_PIN);

    // Disable ADC to save power
    ADC_Cmd(ADC1, DISABLE);

    return Intensity;
}

/**
 * @brief  ADC interrupt function.
 * @param  None.
 * @retval None.
 */
void ADC_IRQHandler(void)
{
    static uint8_t num = 0;
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
    {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        Data_Object[num] = ADC_GetConversionValue(ADC1);
        num++;
        if (num >= 10)
            num = 0;
    }
}

/***********************************END OF FILE********************************/
