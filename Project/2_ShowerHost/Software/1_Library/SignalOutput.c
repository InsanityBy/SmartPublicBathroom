/**
 ******************************************************************************
 * @file    SignalOutput.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.8
 * @brief   This file contains all the functions to control fan, light.(For
 *         shower host.)
 * @note    Follow steps to use.
 *          - Use Fan_Init() or Light_Init() to initialize device.
 *          - Use Vale_Start() or Light_Start() to turn on device.
 *          - Use Valve_Stop() or Light_Stop() to tuen off device.
 *          - Use Valve_GetState() or Light_GetState() to get current state of
 *              device.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "SignalOutput.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Fan output pin - PA3
#define FAN_CLOCK RCC_AHB1Periph_GPIOA
#define FAN_PINGROUP GPIOA
#define FAN_PIN GPIO_Pin_3

// Light output pin - PA4
#define LIGHT_CLOCK RCC_AHB1Periph_GPIOA
#define LIGHT_PINGROUP GPIOA
#define LIGHT_PIN GPIO_Pin_4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Fan state, 1 for on, 0 for off
uint8_t Fan_State = 0x00;

// Light state, 1 for on, 0 for off
uint8_t Light_State = 0x00;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize fan control pin.
 * @param  None.
 * @retval None.
 */
void Fan_Init(void)
{
    RCC_AHB1PeriphClockCmd(FAN_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = FAN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(FAN_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Start fan.
 * @param  None.
 * @retval None.
 */
void Fan_Start(void)
{
    Fan_State = 0x01;
    GPIO_SetBits(FAN_PINGROUP, FAN_PIN);
}

/**
 * @brief  Stop fan.
 * @param  None.
 * @retval None.
 */
void Fan_Stop(void)
{
    Fan_State = 0x00;
    GPIO_ResetBits(FAN_PINGROUP, FAN_PIN);
}

/**
 * @brief  Get fan state.
 * @param  None.
 * @retval Fan state, 1 for on, 0 for off.
 */
uint8_t Fan_GetState(void)
{
    return Fan_State;
}

/**
 * @brief  Initialize light control pin.
 * @param  None.
 * @retval None.
 */
void Light_Init(void)
{
    RCC_AHB1PeriphClockCmd(LIGHT_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = LIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LIGHT_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Start light.
 * @param  None.
 * @retval None.
 */
void Light_Start(void)
{
    Light_State = 0x01;
    GPIO_SetBits(LIGHT_PINGROUP, LIGHT_PIN);
}

/**
 * @brief  Stop light.
 * @param  None.
 * @retval None.
 */
void Light_Stop(void)
{
    Light_State = 0x00;
    GPIO_ResetBits(LIGHT_PINGROUP, LIGHT_PIN);
}

/**
 * @brief  Get light state.
 * @param  None.
 * @retval Light state, 1 for on, 0 for off.
 */
uint8_t Light_GetState(void)
{
    return Light_State;
}

/**
 * @brief  Initialize fan and light control pins.
 * @param  None.
 * @retval None.
 */
void SignalOutput_Init(void)
{
    Light_Init();
    Fan_Init();
}

/***********************************END OF FILE********************************/
