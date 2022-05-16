/**
 ******************************************************************************
 * @file    main.c
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   Main program body.(For shower terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t Data_Human;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void USART1_Init(void)
{
    // GPIO Config
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // GPIO Clock
    GPIO_InitTypeDef GPIO_USART;                          // GPIO Init Structure
    // USART1 TX - PA9
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_USART.GPIO_Pin = GPIO_Pin_9;
    GPIO_USART.GPIO_Mode = GPIO_Mode_AF;
    GPIO_USART.GPIO_OType = GPIO_OType_PP;
    GPIO_USART.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_USART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_USART);
    // USART1 RX - PA10
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_USART.GPIO_Pin = GPIO_Pin_10;
    GPIO_USART.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_USART);
    // USART1 Config
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART Clock
    USART_InitTypeDef USART_InitStructure;                 // USART Init Structure
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void USART1_SendData(uint8_t Data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART1, Data);
}

int fputc(int ch, FILE *f)
{
    USART1_SendData((uint8_t)ch);
    return (ch);
}

void Program_Timer()
{
    // Timer initialize, count period 1800us max
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 0xFFFF;
    TIM_InitStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseInit(TIM9, &TIM_InitStructure);
    TIM_Cmd(TIM9, ENABLE);
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize all peripherals*/
    Delay_us(1000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART1_Init();
    // TemperatureSensor_Init();
    // FlowMeter_Init();
    // Valve_Init();
    // Button_Init();
     InfraredObject_Init();
    // InfraredHuman_Init();
    // NFC_Init();
    Program_Timer();
    Delay_ms(1);
    printf("Initialize finished\n");

    // FlowMeter_Start();

    /* Infinite loop */
    while (1)
    {
        // uint8_t Card_Data[16];
        // Sensor_Convert();
        // Valve_Start();
        // printf("Valve:%d\n", Valve_GetState());

        // Delay_ms(1000);
        // Valve_Stop();
        // printf("Temperature: %f\n", Sensor_GetTemperature());
        // printf("Flow: %fL\n", FlowMeter_GetValueL());
        // printf("Valve: %d\n", Valve_GetState());
         printf("InfraredObjectState: %d\n", InfraredObject_GetState());
         printf("InfraredObjectValue: %f\n", (float)InfraredObject_GetValue() / 4096 * 3.3);
        // InfraredObject_GetValue();
        // printf("InfraredHumanState: %d\n", InfraredHuman_GetState());
        // printf("InfraredHumanValue: %d\n", InfraredHuman_GetValue());
        // NFC_GetData(Card_Data);
        Delay_ms(2000);
    }
}

/***********************************END OF FILE********************************/
