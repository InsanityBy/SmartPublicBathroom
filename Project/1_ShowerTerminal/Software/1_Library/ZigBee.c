/**
 ******************************************************************************
 * @file    ZigBee.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.29
 * @brief   This file contains all the functions for communicate between shower
 *          host and shower terminal by ZigBee.(For shower terminal.)
 * @note    Follow steps to use.
 *          - Use ZigBee_Init(uint8_t *pTransmitData, uint8_t *pReceiveData) to
 *              initialize ZigBee device.
 *          - Use ZigBee_TransmitByte(uint8_t data) to send 1 byte data.
 *          - Use ZigBee_TransmitString(uint16_t length, uint16_t nTime) to send
 *              string.
 *          - Use ZigBee_ReceiveByte() to receive 1 byte data.
 *          - Use ZigBee_ReceiveString(uint16_t nTime) to receive string.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ZigBee.h"
#include "Delay.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// ZigBee RX pin - PA9
#define ZIGBEERX_CLOCK RCC_AHB1Periph_GPIOA
#define ZIGBEERX_PINGROUP GPIOA
#define ZIGBEERX_PIN GPIO_Pin_9
#define ZIGBEERX_PINSOURCE GPIO_PinSource9

// ZigBee TX  - PA10
#define ZIGBEETX_CLOCK RCC_AHB1Periph_GPIOA
#define ZIGBEETX_PINGROUP GPIOA
#define ZIGBEETX_PIN GPIO_Pin_10
#define ZIGBEETX_PINSOURCE GPIO_PinSource10

#define ZIGBEETIMEOUT 500

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// String receive flag, 1 for finished, 0 for not
uint8_t ZigBeeStringReceive_Flag = 0;

// String transmit flag, 1 for finished, 0 for not
uint8_t ZigBeeStringTransmit_Flag = 1;

// Length of received data
uint16_t ZigBeeDataLength;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize USART1 for communicate with ZigBee module.
 * @param  None.
 * @retval None.
 */
void USART1_Init(void)
{
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

    // Enable IDLE interrupt for DMA
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    // NVIC config for USART1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable RX and TX request for DMA
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  Initialize DMA for data reception.
 * @param  pData: Pointer of data buffer.
 * @retval None.
 */
void USART1_DMAReceiveInit(uint8_t *pData)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // Enable DMA2 clock
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          // Use channel4
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // Direction
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // Normal mode, not circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // Priority
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO not use
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           // Invalid
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1 + 0x0004;   // USART1 data registor
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pData;        // Receive buffer
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_BufferSize = 0xFFFF; // Max buffer size
    DMA_Init(DMA2_Stream5, &DMA_InitStructure);

    // Interrupt
    DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
    DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);

    // NVIC config for DMA2 Stream7
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable DMA
    DMA_Cmd(DMA2_Stream5, ENABLE);
}

/**
 * @brief  Initialize DMA for data transmit.
 * @param  pData: Transmit data buffer.
 * @retval None.
 */
void USART1_DMATransmitInit(uint8_t *pData)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // Enable DMA2 clock
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          // Use channel4
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // Direction
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // Normal mode, not circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // Priority
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO not use
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           // Invalid
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1 + 0x0004;   // USART1 data registor
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pData;        // Receive buffer
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // uint8_t
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_BufferSize = 0xFFFF; // Default buffer size, max
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);

    // Interrupt
    DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);

    // NVIC config for DMA2 Stream7
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
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
void ZigBee_Init(uint8_t *pTransmitData, uint8_t *pReceiveData)
{
    // GPIO initialize
    GPIO_InitTypeDef GPIO_InitStructure;
    // ZigBee RX - USART1 TX - PA9
    RCC_AHB1PeriphClockCmd(ZIGBEERX_CLOCK, ENABLE);                          // Enable clock
    GPIO_PinAFConfig(ZIGBEERX_PINGROUP, ZIGBEERX_PINSOURCE, GPIO_AF_USART1); // PA9 -> USART1 TX
    GPIO_InitStructure.GPIO_Pin = ZIGBEERX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ZIGBEERX_PINGROUP, &GPIO_InitStructure);

    // ZigBee TX - USART1 RX - PA10
    RCC_AHB1PeriphClockCmd(ZIGBEETX_CLOCK, ENABLE);                          // Enable clock
    GPIO_PinAFConfig(ZIGBEETX_PINGROUP, ZIGBEETX_PINSOURCE, GPIO_AF_USART1); // PA10 -> USART1 RX
    GPIO_InitStructure.GPIO_Pin = ZIGBEETX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(ZIGBEETX_PINGROUP, &GPIO_InitStructure);

    USART1_Init();
    USART1_DMAReceiveInit(pReceiveData);
    USART1_DMATransmitInit(pTransmitData);
}

/**
 * @brief  Transmit 1 byte data by USART.
 * @param  data: 1 byte data to transmit.
 * @retval None.
 */
void ZigBee_TransmitByte(uint8_t data)
{
    // Wait till TX buffer empty
    uint16_t wait = 0;
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        wait++;
        if (wait > ZIGBEETIMEOUT)
            return;
    }
    // Transmit data
    USART_SendData(USART1, data);
}

/**
 * @brief  Transmit string by USART.
 * @param  length: Length of data.
 * @param  nTime: Wait nTime ms.
 * @retval None.
 */
void ZigBee_TransmitString(uint16_t length, uint16_t nTime)
{
    // Length = 0, no need to transmit
    if (length == 0)
        return;

    // Wait till last transmit finish
    for (int i = 0; i < nTime; i++)
    {
        if (ZigBeeStringTransmit_Flag)
            break;
        Delay_ms(1);
    }

    //  Last transmit time out, return
    if (!ZigBeeStringTransmit_Flag)
        return;

    // Initialize DMA
    DMA_Cmd(DMA2_Stream7, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Stream7, length);

    // Enable DMA
    ZigBeeStringTransmit_Flag = 0x00; // Transmitting...
    DMA_Cmd(DMA2_Stream7, ENABLE);
}

/**
 * @brief  Receive 1 byte data by USART.
 * @param  None.
 * @retval 1 byte data received.
 */
uint8_t ZigBee_ReceiveByte(void)
{
    // Wait till RX buffer not empty
    uint16_t wait = 0;
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {
        wait++;
        if (wait > ZIGBEETIMEOUT)
            return 0x00;
    }
    // Receive data
    return USART_ReceiveData(USART1);
}

/**
 * @brief  Receive string by USART.
 * @param  nTime: Wait nTime us.
 * @retval Length of the ddta.
 */
uint16_t ZigBee_ReceiveString(uint16_t nTime)
{
    // Wait till receive finish
    for (int i = 0; i < nTime; i++)
    {
        if (ZigBeeStringReceive_Flag)
            break;
        Delay_ms(1);
    }

    // Last receive time out
    if (!ZigBeeStringReceive_Flag)
        return ZigBeeDataLength;

    ZigBeeStringReceive_Flag = 0x00;

    return ZigBeeDataLength;
}

/**
 * @brief  USART1 interrupt function. Handle RXNE and IDLE interrupt.
 * @param  None.
 * @retval None.
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        ZigBeeDataLength = 0xFFFF - DMA_GetCurrDataCounter(DMA2_Stream5);
        DMA_Cmd(DMA2_Stream5, DISABLE);
        uint8_t clear = USART_ReceiveData(USART1); // IDLE flag is cleared by reading SR and DR
    }
}

/**
 * @brief  DMA interrupt for transmit finish.
 * @param  None.
 * @retval None.
 */
void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        ZigBeeStringTransmit_Flag = 0x01;
    }
}

/**
 * @brief  DMA interrupt for receive finish.
 * @param  None.
 * @retval None.
 */
void DMA2_Stream5_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
        ZigBeeStringReceive_Flag = 0x01;
        DMA_Cmd(DMA2_Stream5, ENABLE);
    }
}
/***********************************END OF FILE********************************/
