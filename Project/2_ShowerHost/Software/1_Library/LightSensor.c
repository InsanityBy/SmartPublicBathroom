/**
 ******************************************************************************
 * @file    LightSensor.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions for light sensor.(For shower
 *         host.)
 * @note    Follow steps to use.
 *          - Use LightSensor_Init() to initialize device.
 *          - Use LightSensor_GetState() to get current environment light state.
 *          - Use LightSensor_GetValue() to get specific ADC sample value.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "LightSensor.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Light sensor input pin - PC2
#define LIGHTSENSOR_CLOCK RCC_AHB1Periph_GPIOC
#define LIGHTSENSOR_PINGROUP GPIOC
#define LIGHTSENSOR_PIN GPIO_Pin_2

// Use ADC1, Channel 12
#define ADC_CHANNEL ADC_Channel_12

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Light threshold
float Light_Threshold = 2.8;

// Ratio to convert ADC sample value to actual lighting value
float ConvertRatio = 3.3 / 4096;

// Sample 10 times and get average value
uint16_t Data[10];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize input pin and ADC to sample.
 * @param  Threshold: Light threshold, 0 ~ 3.3.
 * @retval None.
 */
void LightSensor_Init(float Threshold)
{
    Light_Threshold = Threshold;

    // GPIO config
    RCC_AHB1PeriphClockCmd(LIGHTSENSOR_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = LIGHTSENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LIGHTSENSOR_PINGROUP, &GPIO_InitStructure);

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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

/**
 * @brief  Get light state.
 * @param  None.
 * @retval 1 for environment is dark and light should be on, 0 for bright and off.
 */
uint8_t LightSensor_GetState(void)
{
    if (LightSensor_GetValue() > Light_Threshold)
        return 0x00;
    else
        return 0x01;
}

/**
 * @brief  Get specific lighting value.
 * @param  None.
 * @retval Accurate lighting value.
 */
float LightSensor_GetValue(void)
{
    uint16_t Light_Original = 0;
    // Enable ADC
    ADC_Cmd(ADC1, ENABLE);

    for (int i = 0; i < 10; i++)
    {
        ADC_SoftwareStartConv(ADC1);
        Delay_us(1);
    }
    for (int i = 0; i < 10; i++)
    {
        Light_Original += Data[i];
    }

    // Disable ADC to save power
    ADC_Cmd(ADC1, DISABLE);

    return (float)Light_Original * ConvertRatio / 10;
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
        Data[num] = ADC_GetConversionValue(ADC1);
        num++;
        if (num >= 10)
            num = 0;
    }
}

/***********************************END OF FILE********************************/
