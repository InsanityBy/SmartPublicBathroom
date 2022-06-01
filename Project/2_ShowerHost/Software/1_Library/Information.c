/**
 ******************************************************************************
 * @file    Information.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.30
 * @brief   This file contains all the functions of device information.(For
 *         shower host.)
 * @note    Follow steps to use.
 *          - Use Information_InitializeRTC() to initialize RTC.
 *          - Use Information_SetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef
 *              *Time) to set date and time.
 *          - Use Information_SetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef
 *              *Time) to get current date and time.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Information.h"
#include <time.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize RTC.
 * @param  None.
 * @retval None
 */
void Information_InitializeRTC(void)
{
    // Access backup domain
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    // Set clock source
    RCC_LSEConfig(RCC_LSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        ;
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();

    // Set prescalar and time format
    RTC_WriteProtectionCmd(DISABLE);
    RTC_EnterInitMode();
    RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7D - 1;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF - 1;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    // Set date
    RTC_DateTypeDef RTC_DateStructure;
    RTC_DateStructure.RTC_Year = 22;
    RTC_DateStructure.RTC_Month = 5;
    RTC_DateStructure.RTC_Date = 30;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

    // Set time
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_TimeStructure.RTC_Hours = 8;
    RTC_TimeStructure.RTC_Minutes = 0;
    RTC_TimeStructure.RTC_Seconds = 0;
    RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

    RTC_ExitInitMode();
    RTC_WriteProtectionCmd(ENABLE);
    PWR_BackupAccessCmd(DISABLE);
}

/**
 * @brief  Set date and time.
 * @param  Date: New date to set.
 * @param  Time: New time to set.
 * @retval None.
 */
void Information_SetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef *Time)
{
    // Access backup domain
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    // Set
    RTC_SetDate(RTC_Format_BIN, Date);
    RTC_SetTime(RTC_Format_BIN, Time);

    PWR_BackupAccessCmd(DISABLE);
}

/**
 * @brief  Get date and time.
 * @param  Date: New date to set.
 * @param  Time: New time to set.
 * @retval None.
 */
void Information_GetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef *Time)
{
    RTC_GetDate(RTC_Format_BIN, Date);
    RTC_GetTime(RTC_Format_BIN, Time);
}

/**
 * @brief  Get time stamp in seconds.
 * @param  None.
 * @retval Seconds from 1970.1.1 00:00:00 to now.
 */
uint32_t Information_GetTimeStamp(void)
{
    struct tm tm_new;
    RTC_DateTypeDef CurrentDate; // Current date
    RTC_TimeTypeDef CurrentTime; // Current time
    Information_GetDateTime(&CurrentDate, &CurrentTime);
    tm_new.tm_year = CurrentDate.RTC_Year + 100;
    tm_new.tm_mon = CurrentDate.RTC_Month - 1;
    tm_new.tm_mday = CurrentDate.RTC_Date;
    tm_new.tm_hour = CurrentTime.RTC_Hours;
    tm_new.tm_min = CurrentTime.RTC_Minutes;
    tm_new.tm_sec = CurrentTime.RTC_Seconds;
    tm_new.tm_isdst = 0;

    return mktime(&tm_new);
}

/***********************************END OF FILE********************************/
