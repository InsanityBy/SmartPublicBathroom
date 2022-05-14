/**
 ******************************************************************************
 * @file    InfraredHuman.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.15
 * @brief   This file contains all the functions for infrared human sensor.(For
 *          shower terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "InfraredHuman.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INFRAREDHUMANRX_CLOCK RCC_AHB1Periph_GPIOA
#define INFRAREDHUMANRX_PINGROUP GPIOA
#define INFRAREDHUMANRX_PIN GPIO_Pin_2
#define INFRAREDHUMANRX_EXTIPINGROUP EXTI_PortSourceGPIOA
#define INFRAREDHUMANRX_EXTIPINSOURCE EXTI_PinSource2
#define INFRAREDHUMANRX_EXTILINE EXTI_Line2

#define INFRAREDHUMANTX_CLOCK RCC_AHB1Periph_GPIOA
#define INFRAREDHUMANTX_PINGROUP GPIOA
#define INFRAREDHUMANTX_PIN GPIO_Pin_8
#define INFRAREDHUMANTX_PINSOURCE GPIO_PinSource8

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Number of correct receptions indicating human presence
uint8_t Threshold_Human = 8;

// Receive flag, 1 for receive finished, 0 for not
uint8_t Receive_Flag = 0;

// Received data
uint8_t Data_Human = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize timer TIM1 to generate PWM and output pin to emit infrared.
 * @param  None.
 * @retval None.
 */
void InfraredHumanTX_Init(void)
{
    // Initialize timer TIM1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // Enable clock
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // CK_PSC = CK_INT = APB2 clock = 84MHz
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 220 - 1;   // PWM frequency = CK_CNT/220 = 38kHz
    TIM_InitStructure.TIM_Prescaler = 10 - 1; // CK_CNT = CK_PSC/10 = 8.4MHz
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_InitStructure);

    // Initialize timer channel output mode
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = 110; // Set duty of PWM, duty = pulse / period
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // Disable complementary output
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // GPIO config
    RCC_AHB1PeriphClockCmd(INFRAREDHUMANTX_CLOCK, ENABLE); // Enable clock
    GPIO_PinAFConfig(INFRAREDHUMANTX_PINGROUP, INFRAREDHUMANTX_PINSOURCE, GPIO_AF_TIM1);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INFRAREDHUMANTX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(INFRAREDHUMANTX_PINGROUP, &GPIO_InitStructure);
    GPIO_SetBits(INFRAREDHUMANTX_PINGROUP, INFRAREDHUMANTX_PIN);
}

/**
 * @brief  Initialize input pin to receive data.
 * @param  None.
 * @retval None.
 */
void InfraredHumanRX_Init(void)
{
    // GPIO config
    RCC_AHB1PeriphClockCmd(INFRAREDHUMANRX_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INFRAREDHUMANRX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(INFRAREDHUMANRX_PINGROUP, &GPIO_InitStructure);

    // Input interrupt config
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(INFRAREDHUMANRX_EXTIPINGROUP, INFRAREDHUMANRX_EXTIPINSOURCE);
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = INFRAREDHUMANRX_EXTILINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC config
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Initialize TIM5 for receive data. TIM5 is used to measure 1 bit data
 *          duration time
 * @param  None.
 * @retval None.
 */
void TIM5_Init(void)
{
    // Timer initialize, count period 1800us max
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 0xFFFF;
    TIM_InitStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseInit(TIM5, &TIM_InitStructure);
}

/**
 * @brief  Initialize input and output pin.
 * @param  None.
 * @retval None.
 */
void InfraredHuman_Init(void)
{
    InfraredHumanRX_Init();
    InfraredHumanTX_Init();
    TIM5_Init();
}

/**
 * @brief  Emit infrared pulse.
 * @param  None.
 * @retval None.
 */
void InfraredHuman_TransmitPulse(void)
{
    TIM_Cmd(TIM1, ENABLE);
    Delay_us(450);
    TIM_Cmd(TIM1, DISABLE);
}

/**
 * @brief  Emit infrared to transmit 1 byte data.
 * @param  data: data to transmit.
 * @retval None.
 */
void InfraredHuman_Transmit(uint8_t data)
{
    // Reset receive timer
    TIM_Cmd(TIM5, DISABLE);
    TIM_SetCounter(TIM5, 0x00);

    // Transmit start bit
    InfraredHuman_TransmitPulse();
    Delay_us(1350);

    // Transmit data
    for (int i = 0; i < 8; i++)
    {
        if (data & 0x01) // Transmit "1"
        {
            InfraredHuman_TransmitPulse();
            Delay_us(450);
        }
        else // Transmit "0"
        {
            InfraredHuman_TransmitPulse();
            Delay_us(900);
        }
        data = data >> 1;
    }

    // Transmit Stop pulse
    InfraredHuman_TransmitPulse();
    Delay_us(450);
}

/**
 * @brief  Receive infrared sensor data.
 * @param  None.
 * @retval Data from sensor.
 */
uint8_t InfraredHuman_Receive(void)
{
    uint8_t temp = Data_Human;
    Data_Human = 0x00;   // Clear buffer
    Receive_Flag = 0x00; // Clear flag
    return temp;
}

/**
 * @brief  Check received data.
 * @param  data: transmit data to compare.
 * @retval 1 for correct, 0 for error.
 */
uint8_t InfraredHuman_Check(uint8_t data)
{
    for (int i = 0; i < 10; i++) // More than 10us, receive failure
    {
        if (Receive_Flag) // Received data
        {
            // Check received data
            if (InfraredHuman_Receive() == data)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        Delay_us(1);
    }
    return 0;
}

/**
 * @brief  Get correct reception number.
 * @param  None.
 * @retval Number of correct reception.
 */
uint8_t InfraredHuman_GetValue(void)
{
    uint16_t Number = 0;

    for (int i = 0; i < 10; i++) // Check 10 times
    {
        // Every time send different data
        uint8_t data;
        switch (i % 4)
        {
        case 0:
            data = 0x55;
            break;
        case 1:
            data = 0xCC;
            break;
        case 2:
            data = 0xAA;
            break;
        case 3:
            data = 0x33;
            break;
        default:
            break;
        }
        
        InfraredHuman_Transmit(data);
        if (InfraredHuman_Check(data))
            Number++;
    }
    return Number;
}

/**
 * @brief  Get human state.
 * @param  None.
 * @retval 1 for human presence, 0 for not.
 */
uint8_t InfraredHuman_GetState(void)
{
    // Check threshold
    if (InfraredHuman_GetValue() > Threshold_Human)
        return 1;
    else
        return 0;
}

/**
 * @brief  Receive interrupt function.
 * @param  None.
 * @retval None.
 */
void EXTI2_IRQHandler(void)
{
    static uint8_t num = 0;
    static uint8_t data_buffer = 0;
    static uint8_t receive_start = 0;

    if (EXTI_GetITStatus(INFRAREDHUMANRX_EXTILINE) != RESET)
    {
        EXTI_ClearITPendingBit(INFRAREDHUMANRX_EXTILINE);
        uint16_t time = TIM_GetCounter(TIM5);
        TIM_Cmd(TIM5, DISABLE);
        TIM_SetCounter(TIM5, 0x00);
        TIM_Cmd(TIM5, ENABLE); // Start TIM5 to sample

        if (time < 1900 && time > 1700) // Start bit
        {
            receive_start = 1;
            num = 0;
            data_buffer = 0;
        }
        else if (time < 1450 && time > 1250) // "0"
        {
            if (receive_start)
            {
                data_buffer = data_buffer >> 1;
                data_buffer = data_buffer | 0x00;
                num++;
            }
        }
        else if (time < 1000 && time > 800) // "1"
        {
            if (receive_start)
            {
                data_buffer = data_buffer >> 1;
                data_buffer = data_buffer | 0x80;
                num++;
            }
        }
        if (num >= 8)
        {
            TIM_Cmd(TIM5, DISABLE);
            TIM_SetCounter(TIM5, 0x00);

            Data_Human = data_buffer;
            data_buffer = 0x00;

            num = 0;
            receive_start = 0;
            Receive_Flag = 1;
        }
    }
}

/***********************************END OF FILE********************************/