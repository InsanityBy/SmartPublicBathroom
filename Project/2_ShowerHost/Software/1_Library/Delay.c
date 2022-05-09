/**
  ******************************************************************************
  * @file    Delay.c
  * @author  Ma boyang
  * @version V1.0
  * @date    2022.5.7
  * @brief   This file contains all the functions of delay using systick.(For
  *         shower host.)
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Delay n us.
 * @param  n: Delay n us.
 * @retval None.
 */
void Delay_us(uint32_t n)
{
    SysTick->LOAD = SystemCoreClock / 8000000 * n; // Reload value, SysTick clock is HCLK/8
    SysTick->VAL = 0x00000000;                     // Clear current value
    SysTick->CTRL = 0x00000001;                    // Enable counter
    uint32_t temp = SysTick->CTRL;                 // Get current CTRL value
    while ((temp & 0x01) && (!(temp & (1 << 16)))) // Wait till 0
    {
        temp = SysTick->CTRL;
    }
    SysTick->CTRL = 0x00000000; // Disable counter
    SysTick->VAL = 0x00000000;  // Clear current value
}

/**
 * @brief  Delay n ms.
 * @param  n: Delay n ms.
 * @retval None.
 */
void Delay_ms(uint32_t n)
{
    SysTick->LOAD = SystemCoreClock / 8000 * n;    // Reload value, SysTick clock is HCLK/8
    SysTick->VAL = 0x00000000;                     // Clear current value
    SysTick->CTRL = 0x00000001;                    // Enable counter
    uint32_t temp = SysTick->CTRL;                 // Get current CTRL value
    while ((temp & 0x01) && (!(temp & (1 << 16)))) // Wait till 0
    {
        temp = SysTick->CTRL;
    }
    SysTick->CTRL = 0x00000000; // Disable counter
    SysTick->VAL = 0x00000000;  // Clear current value
}

/***********************************END OF FILE********************************/
