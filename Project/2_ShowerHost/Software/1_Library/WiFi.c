/**
 ******************************************************************************
 * @file    WiFi.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.17
 * @brief   This file contains all the functions for communicate between shower
 *          host and shower terminal by WiFi.(For shower host.)
 * @note    Follow steps to use.
 *          - Use WiFi_Init(uint8_t *pTransmitData, uint8_t *pReceiveData) to
 *              initialize WiFi device.
 *          - Use WiFi_TransmitByte(uint8_t data) to send 1 byte data.
 *          - Use WiFi_TransmitString(uint16_t length, uint16_t nTime) to send
 *              string.
 *          - Use WiFi_ReceiveByte() to receive 1 byte data.
 *          - Use WiFi_ReceiveString(uint16_t nTime) to receive string.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "WiFi.h"
#include "Delay.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// WiFi RX pin - PC6
#define WIFIRX_CLOCK RCC_AHB1Periph_GPIOC
#define WIFIRX_PINGROUP GPIOC
#define WIFIRX_PIN GPIO_Pin_6
#define WIFIRX_PINSOURCE GPIO_PinSource6

// WiFi TX  - PC7
#define WIFITX_CLOCK RCC_AHB1Periph_GPIOC
#define WIFITX_PINGROUP GPIOC
#define WIFITX_PIN GPIO_Pin_7
#define WIFITX_PINSOURCE GPIO_PinSource7

#define WIFITIMEOUT 500

// AT Command
#define WiFi_DeviceReset "AT+UT_RESET\r\n"

#define WiFi_Version "AT+UT_VER=?\r\n"

#define WiFi_WorkMode "AT+UT_WKMODE="
#define WiFi_WorkModeAT "CMD"
#define WiFi_WorkModeTransparent "NET"

#define WiFi_WiFiConfig "AT+WIFI="
#define WiFi_WiFiModeSTA "STA"
#define WiFi_WiFiModeAP "AP"

#define WiFi_Socket1EN "AT+ETH_CH1EN="
#define WiFi_Socket2EN "AT+ETH_CH2EN="
#define WiFi_Enable "ENABLE"
#define WiFi_Disable "DISABLE"

#define WiFi_Socket1Config "AT+ETH_CH1P="
#define WiFi_Socket2Config "AT+ETH_CH2P="

#define WiFi_SocketUDPClient "UDPC"
#define WiFi_SocketUDPService "UDPS"
#define WiFi_SocketTCPClient "TCPC"
#define WiFi_SocketTCPService "TCPS"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// String receive flag, 1 for finished, 0 for not
uint8_t WiFiStringReceive_Flag = 1;

// String transmit flag, 1 for finished, 0 for not
uint8_t WiFiStringTransmit_Flag = 1;

// Length of received data
uint16_t WiFiDataLength;

// WiFi information
uint8_t WiFi_Name[] = "insanity";
uint8_t WiFi_Password[] = "20010120";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize USART6 for communicate with WiFi module.
 * @param  None.
 * @retval None.
 */
void USART6_Init(void)
{
    // USART6 Config
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); // USART Clock
    USART_InitTypeDef USART_InitStructure;                 // USART Init Structure
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &USART_InitStructure);

    // Enable IDLE interrupt for DMA
    USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);

    // NVIC config for USART6
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable RX and TX request for DMA
    USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);

    // Enable USART6
    USART_Cmd(USART6, ENABLE);
}

/**
 * @brief  Initialize DMA for data reception.
 * @param  pData: Pointer of data buffer.
 * @retval None.
 */
void USART6_DMAReceiveInit(uint8_t *pData)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // Enable DMA2 clock
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;                          // Use channel5
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // Direction
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // Normal mode, not circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // Priority
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO not use
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           // Invalid
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART6 + 0x0004;   // USART6 data registor
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pData;        // Receive buffer
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_BufferSize = 0xFFFF; // Max buffer size
    DMA_Init(DMA2_Stream1, &DMA_InitStructure);

    // Interrupt
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
    DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);

    // NVIC config for DMA2 Stream1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable DMA
    DMA_Cmd(DMA2_Stream1, ENABLE);
}

/**
 * @brief  Initialize DMA for data transmit.
 * @param  pData: Transmit data buffer.
 * @retval None.
 */
void USART6_DMATransmitInit(uint8_t *pData)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // Enable DMA2 clock
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;                          // Use channel5
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // Direction
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // Normal mode, not circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // Priority
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO not use
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           // Invalid
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART6 + 0x0004;   // USART6 data registor
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pData;        // Receive buffer
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_BufferSize = 0xFFFF; // Default buffer size, max
    DMA_Init(DMA2_Stream6, &DMA_InitStructure);

    // Interrupt
    DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
    DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);

    // NVIC config for DMA2 Stream6
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Initialize GPIO pins, USART and DMA.
 * @param  pTransmitData: Pointer of transmit data buffer.
 * @param  pReceiveData: Pointer of receive data buffer.
 * @retval None.
 */
void WiFi_Init(uint8_t *pTransmitData, uint8_t *pReceiveData)
{
    // GPIO initialize
    GPIO_InitTypeDef GPIO_InitStructure;
    // WiFi RX - USART6 TX - PC6
    RCC_AHB1PeriphClockCmd(WIFIRX_CLOCK, ENABLE);                        // Enable clock
    GPIO_PinAFConfig(WIFIRX_PINGROUP, WIFIRX_PINSOURCE, GPIO_AF_USART6); // PC6 -> USART6 TX
    GPIO_InitStructure.GPIO_Pin = WIFIRX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WIFIRX_PINGROUP, &GPIO_InitStructure);

    // WiFi TX - USART6 RX - PC7
    RCC_AHB1PeriphClockCmd(WIFITX_CLOCK, ENABLE);                        // Enable clock
    GPIO_PinAFConfig(WIFITX_PINGROUP, WIFITX_PINSOURCE, GPIO_AF_USART6); // PC7 -> USART6 RX
    GPIO_InitStructure.GPIO_Pin = WIFITX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(WIFITX_PINGROUP, &GPIO_InitStructure);

    USART6_Init();
    USART6_DMAReceiveInit(pReceiveData);
    USART6_DMATransmitInit(pTransmitData);
}

/**
 * @brief  Transmit 1 byte data by USART.
 * @param  data: 1 byte data to transmit.
 * @retval None.
 */
void WiFi_TransmitByte(uint8_t data)
{
    // Wait till TX buffer empty
    uint16_t wait;
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)
    {
        wait++;
        if (wait > WIFITIMEOUT)
            return;
    }
    // Transmit data
    USART_SendData(USART6, data);
}

/**
 * @brief  Transmit string by USART.
 * @param  length: Length of data.
 * @param  nTime: Wait nTime us.
 * @retval None.
 */
void WiFi_TransmitString(uint16_t length, uint16_t nTime)
{
    // Length = 0, no need to transmit
    if (length == 0)
        return;

    // Wait till last transmit finish
    for (int i = 0; i < nTime; i++)
    {
        if (WiFiStringTransmit_Flag)
            break;
        Delay_us(1);
    }

    //  Last transmit time out, return
    if (!WiFiStringTransmit_Flag)
        return;

    // Initialize DMA
    DMA_Cmd(DMA2_Stream6, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Stream6, length);

    // Enable DMA
    WiFiStringTransmit_Flag = 0x00; // Transmitting...
    DMA_Cmd(DMA2_Stream6, ENABLE);
}

/**
 * @brief  Receive 1 byte data by USART.
 * @param  None.
 * @retval 1 byte data received.
 */
uint8_t WiFi_ReceiveByte(void)
{
    // Wait till RX buffer not empty
    uint16_t wait = 0;
    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET)
    {
        wait++;
        if (wait > WIFITIMEOUT)
            return 0x00;
    }
    // Receive data
    return USART_ReceiveData(USART6);
}

/**
 * @brief  Receive string by USART.
 * @param  nTime: Wait nTime us.
 * @retval Length of the ddta.
 */
uint16_t WiFi_ReceiveString(uint16_t nTime)
{
    // Wait till receive finish
    for (int i = 0; i < nTime; i++)
    {
        if (WiFiStringReceive_Flag)
            break;
        Delay_us(1);
    }

    // Last receive time out
    if (!WiFiStringReceive_Flag)
        return WiFiDataLength;

    WiFiStringReceive_Flag = 0x00;

    // Enable DMA
    DMA_Cmd(DMA2_Stream1, ENABLE);

    return WiFiDataLength;
}

/**
 * @brief  USART6 interrupt function. Handle RXNE and IDLE interrupt.
 * @param  None.
 * @retval None.
 */
void USART6_IRQHandler(void)
{
    if (USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
    {
        WiFiDataLength = 0xFFFF - DMA_GetCurrDataCounter(DMA2_Stream1);
        DMA_Cmd(DMA2_Stream1, DISABLE);
        uint8_t clear = USART_ReceiveData(USART6); // IDLE flag is cleared by reading SR and DR
    }
}

/**
 * @brief  DMA interrupt for transmit finish.
 * @param  None.
 * @retval None.
 */
void DMA2_Stream6_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
        WiFiStringTransmit_Flag = 0x01;
    }
}

/**
 * @brief  DMA interrupt for receive finish.
 * @param  None.
 * @retval None.
 */
void DMA2_Stream1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
        WiFiStringReceive_Flag = 0x01;
    }
}
/***********************************END OF FILE********************************/
