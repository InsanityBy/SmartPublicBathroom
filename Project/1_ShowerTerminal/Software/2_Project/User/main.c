/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
 * @author  MCD Application Team
 * @version V1.8.0
 * @date    04-November-2016
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
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
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize all peripherals*/
    Delay_us(1000);
    USART1_Init();
    if (TemperatureSensor_Init())
        printf("ERROR\n");
    FlowMeter_Init();
    Delay_ms(1);

			FlowMeter_Start();

    /* Infinite loop */
    while (1)
    {
        Sensor_Convert();
        Delay_ms(1000);
        float Flow = FlowMeter_GetValueL();
        float Temperature = Sensor_GetTemperature();
        //printf("Temperature: %f\n", Temperature);
        printf("Flow:%f\n", Flow);
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
