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
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize all peripherals*/
    ShowerTerminal_Init();

    /* Infinite loop */
    while (1)
    {
        //ShowerTerminal_GetZigBeeData();
        ShowerTerminal_SetDevice();

        if (ShowerTerminal_ObjectLeftCheck())
        {
            ShowerTerminal_TakeObjectProcess();
        }
        else if (ShowerTerminal_GetRepairState())
        {
            uint8_t Content[] = "Device Failure !";
            ShowerTerminal_DisplayContent(Content);
        }
        else if (ShowerTerminal_GetHelpState())
        {
            uint8_t Content[] = "NEED HELP!";
            ShowerTerminal_DisplayContent(Content);
        }
        else if (ShowerTerminal_HumanDetect()) // Human presence
        {
            if (ShowerTerminal_CardDetect()) // Have card
            {
                if (ShowerTerminal_CardFirstRead()) // First swipe card
                {
                    ShowerTerminal_CardFirstReadProcess();
                }

                if (ShowerTerminal_ReservationCheck()) // Have reservation
                {
                    ShowerTerminal_InUseProcess();
                    ShowerTerminal_WaterStopRemind();
                    ShowerTerminal_NearReservationEnd();
                }
                else // Have no reservation
                {
                    ShowerTerminal_NoReservationProcess();
                }
            }
            else // No card
            {
                if (ShowerTerminal_CardTake()) // Take the card
                {
                    ShowerTerminal_CardTakeProcess();
                }
                else // Not swipe card yet
                {
                    ShowerTerminal_NotUseProcess();
                }
            }
        }
        else // Human absence
        {
            if (ShowerTerminal_CardDetect() || ShowerTerminal_ObjectDetect()) // Have card or object
            {
                ShowerTerminal_CardObjectLeft();
            }
            else
            {
                ShowerTerminal_IdleState();
            }
        }
        //ShowerTerminal_SendZigBeeData();
    }
}
/***********************************END OF FILE********************************/
