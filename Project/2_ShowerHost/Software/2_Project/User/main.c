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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize all peripherals*/
    ShowerHost_Init();

    /* Infinite loop */
    while (1)
    {
        ShowerHost_GetZigBeeData(2);
        ShowerHost_SendWiFiData(2);
        ShowerHost_GetWiFiData(2);
        ShowerHost_SendZigBeeData(2);

        if (ShowerHost_GetRepairState())
        {
            ShowerHost_DisplayRepair();
        }
        else if (ShowerHost_GetHelpState())
        {
            ShowerHost_DisplayHelp();
        }
        else
        {
            ShowerHost_TemperatureDetect();
            ShowerHost_HumidityDetect();
            ShowerHost_FanControl();

            ShowerHost_LightDetect();
            // ShowerHost_LightControl();

            ShowerHost_DisplayNormal();
        }

        Delay_s(1);
    }
}

/***********************************END OF FILE********************************/
