/**
 ******************************************************************************
 * @file    main.c
 * @author  Ma Boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   Main program body.(For shower host.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void USART_TestInit(void)
{
    // GPIO Config
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // GPIO Clock
    GPIO_InitTypeDef GPIO_USART;                          // GPIO Init Structure
    // USART1 TX - PB6
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_USART.GPIO_Pin = GPIO_Pin_6;
    GPIO_USART.GPIO_Mode = GPIO_Mode_AF;
    GPIO_USART.GPIO_OType = GPIO_OType_PP;
    GPIO_USART.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_USART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_USART);

    // USART6 RX - PA12
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART6);
    GPIO_USART.GPIO_Pin = GPIO_Pin_12;
    GPIO_USART.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_USART);

    // USART1 Config
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART Clock
    USART_InitTypeDef USART_InitStructure;                 // USART Init Structure
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void USART_TestSendData(uint8_t Data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART1, Data);
}

int fputc(int ch, FILE *f)
{
    USART_TestSendData((uint8_t)ch);
    return (ch);
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize all peripherals*/
    // char Receive_Buffer[256] = "AT+ZIGB_TYPE=0\r\n";
    // char Transmit_Buffer[] = "AT+UT_VER=?\r\n";
    // char Receive_Buffer[256];
    Delay_us(1000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART_TestInit();
    SignalOutput_Init();
    Button_Init();
    TemperatureHumiditySensor_Init();
    LightSensor_Init();
    // ZigBee_Init(Transmit_Buffer, Receive_Buffer);
    // WiFi_Init(Transmit_Buffer, Receive_Buffer);
    Audio_Init();
    Audio_Volume(15);
    Delay_ms(1);
    printf("Initialize finished!\n");
    // uint16_t temp = sizeof(Transmit_Buffer) / sizeof(Transmit_Buffer[0]) - 1;
    // WiFi_TransmitString(temp, 100);

    /* Infinite loop */
    while (1)
    {
        Delay_ms(2000);
        Audio_Play(0x0005);
        // Sensor_Convert();
        // printf("Temperature: %f\n", Sensor_GetTemperature());
        // printf("Humidity: %f\n", Sensor_GetHumidity());
        // printf("LightState: %d\n", LightSensor_GetState());
        // printf("LightValue: %f\n", LightSensor_GetValue());
        // ZigBee_ReceiveString(Data);
        uint16_t length = WiFi_ReceiveString(100);
    }
}

/***********************************END OF FILE********************************/
