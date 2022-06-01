/**
 ******************************************************************************
 * @file    UserLibrary.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.30
 * @brief   This file contains all the functions for user libraries.(For shower
 *          host.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "UserLibrary.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TerminalMaxNumber 6
#define MAXWIFIDTALENGTH 1024
#define MAXZIGBEELENGTH 128

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Host information
uint8_t ShowerHostSerial = 0x01;
uint8_t ShowerTerminalNumber = 0x00;
ShowerHost_InformationTypeDef Host_Information;

// Terminal information buffer
uint8_t Terminal_InformationFromCloud[TerminalMaxNumber][MAXZIGBEELENGTH];
uint8_t Terminal_InformationFromTerminal[TerminalMaxNumber][MAXZIGBEELENGTH];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize shower host information.
 * @param  None.
 * @retval None.
 */
void ShowerHost_InitInformation(void)
{
    Host_Information.HostSerial = ShowerHostSerial;
    Host_Information.TerminalNumber = ShowerTerminalNumber;
    Host_Information.HostState = 0x0C;
    Host_Information.StateControl = 0x00;
    Host_Information.Light = 0;
    Host_Information.Humidity = 90;
    Host_Information.Temperature = 30;
    Information_GetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);
}

/**
 * @brief  Initialize all peripherals of shower host.
 * @param  None.
 * @retval None.
 */
void ShowerHost_Init(void)
{
    // Wait supply stable
    Delay_ms(10);

    // Initialize date and time recoder
    Information_InitializeRTC();

    // Initialize information
    ShowerHost_InitInformation();

    // Initialize audio
    // Audio_Init();
    Audio_Volume(15);

    // Initialize buttons
    Button_Init();

    // Initialize communicate
    Communicate_Init();

    // Initialize display
    Display_Init();
    Display_Clear();

    // Initialize light sensor
    LightSensor_Init();

    // Initialize signal output
    SignalOutput_Init();

    // Initialize temperature - humdity sensor
    TemperatureHumiditySensor_Init();

    // Display initialization finished
    uint8_t Content[] = {"Initialized!"};
    Display_ShowString(16, 1, Content, 0xFF, FONTSIZE_16);
    Delay_s(1);
    Display_Clear();
}

/**
 * @brief  Get data from cloud server and resolve.
 * @param  None.
 * @retval None.
 */
void ShowerHost_GetWiFiData(void)
{
    uint8_t Data[MAXWIFIDTALENGTH];
    uint8_t ACK[4];
    sprintf(ACK, "A%c@", Host_Information.HostSerial);
    uint8_t NACK[4];
    sprintf(NACK, "N%c@", Host_Information.HostSerial);

    Communicate_WiFiRX(Data);
    if (Data[0] != 'C')
    {
        //Communicate_WiFiTX(NACK);
    }
    Communicate_WiFiRX(Data);
    if (Data[0] != 'C')
    {
        //Communicate_WiFiTX(NACK);
    }
    for (int i = 0; i < MAXWIFIDTALENGTH; i++)
    {
    }
}

void ShowerHost_GetZigBeeData(void)
{
    uint8_t Data[MAXZIGBEELENGTH];
    uint8_t ACK[] = "A$";
    uint8_t NACK[] = "N$";

    uint16_t StartNum;

    Communicate_ZigBeeRX(Data);

    for (int i = 0; i < TerminalMaxNumber; i++)
    {
        if(Data[i]=='C')
        if (Data[1] == Terminal_InformationFromTerminal[i][1]);
    }
}

uint8_t ShowerHost_GetHelpState(void)
{
    if ((Host_Information.HostState & ShowerHost_Help) == ShowerHost_Help)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ShowerHost_SetHelpState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Help;
}

uint8_t ShowerHost_GetRepairState(void)
{
    if ((Host_Information.HostState & ShowerHost_Repair) == ShowerHost_Repair)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ShowerHost_SetRepairState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Repair;
}

void ShowerHost_TemperatureDetect(void)
{
    Sensor_Convert();
    Host_Information.Temperature = Sensor_GetTemperature();
}

void ShowerHost_HumidityDetect(void)
{
    Sensor_Convert();
    Host_Information.Humidity = Sensor_GetHumidity();
}

void ShowerHost_FanControl(void)
{
    // Set by cloud
    if ((Host_Information.StateControl & ShowerHost_Fan) == ShowerHost_Fan)
    {
        return;
    }

    if (Sensor_Getstate())
    {
        if (Fan_GetState() == 0)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Fan_Start();
        Host_Information.HostState = Host_Information.HostState | ShowerHost_Fan;
    }
    else
    {
        if (Fan_GetState() == 1)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Fan_Stop();
        Host_Information.HostState = Host_Information.HostState & (~ShowerHost_Fan);
    }
}

void ShowerHost_LightDetect(void)
{
    Host_Information.Light = LightSensor_GetValue();
}

void ShowerHost_LightControl(void)
{
    // Set by cloud
    if ((Host_Information.StateControl & ShowerHost_Light) == ShowerHost_Light)
    {
        return;
    }

    // Only turn on when night, when not controlled by cloud
    if ((Host_Information.CurrentTime.RTC_Hours > 8) && (Host_Information.CurrentTime.RTC_Hours < 18))
    {
        if (Light_GetState() == 1)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Light_Stop();
    }

    if (LightSensor_GetState())
    {
        if (Light_GetState() == 0)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Light_Start();
    }
    else
    {
        if (Light_GetState() == 1)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Light_Stop();
    }
}

void ShowerHost_SetDevice(void)
{
    // Date & time
    Information_SetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);

    // Fan
    if ((Host_Information.HostState & ShowerHost_Fan) == ShowerHost_Fan)
    {
        if (Fan_GetState() == 0)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Fan_Start();
    }
    else
    {
        if (Fan_GetState() == 1)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Fan_Stop();
    }

    // Light
    if ((Host_Information.HostState & ShowerHost_Light) == ShowerHost_Light)
    {
        if (Light_GetState() == 0)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Light_Start();
    }
    else
    {
        if (Light_GetState() == 1)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Light_Stop();
    }
}

void ShowerHost_DisplayHelp(void)
{
    uint8_t Content[22];

    // Date and time
    sprintf(Content, "%02d%02d%02d %02d:%02d ", Host_Information.CurrentDate.RTC_Year,
            Host_Information.CurrentDate.RTC_Month, Host_Information.CurrentDate.RTC_Date,
            Host_Information.CurrentTime.RTC_Hours, Host_Information.CurrentTime.RTC_Minutes);
    Display_ShowString(0, 0, Content, 22, FONTSIZE_8);

    // Water supply
    if ((Host_Information.HostState & ShowerHost_WaterSupply) == ShowerHost_WaterSupply)
    {
        sprintf(Content, "WaterON");
    }
    else
    {
        sprintf(Content, "NoWater");
    }
    Display_ShowString(78, 0, Content, 22, FONTSIZE_8);

    // Help
    sprintf(Content, "NEED HELP!   ");
    Display_ShowString(24, 1, Content, 22, FONTSIZE_16);

    // Fan and light state and number of terminal
    if ((Host_Information.HostState & ShowerHost_Fan) == ShowerHost_Fan)
    {
        sprintf(Content, "Fan ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Fan OFF", Host_Information.Humidity);
    }
    Display_ShowString(0, 3, Content, 22, FONTSIZE_8);
    if ((Host_Information.HostState & ShowerHost_Light) == ShowerHost_Light)
    {
        sprintf(Content, "Light ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Light OFF", Host_Information.Humidity);
    }
    Display_ShowString(48, 3, Content, 22, FONTSIZE_8);
    sprintf(Content, "%d", Host_Information.TerminalNumber);
    Display_ShowString(108, 3, Content, 22, FONTSIZE_8);
}

void ShowerHost_DisplayRepair(void)
{
    uint8_t Content[22];

    // Date and time
    sprintf(Content, "%02d%02d%02d %02d:%02d ", Host_Information.CurrentDate.RTC_Year,
            Host_Information.CurrentDate.RTC_Month, Host_Information.CurrentDate.RTC_Date,
            Host_Information.CurrentTime.RTC_Hours, Host_Information.CurrentTime.RTC_Minutes);
    Display_ShowString(0, 0, Content, 22, FONTSIZE_8);

    // Water supply
    if ((Host_Information.HostState & ShowerHost_WaterSupply) == ShowerHost_WaterSupply)
    {
        sprintf(Content, "WaterON");
    }
    else
    {
        sprintf(Content, "NoWater");
    }
    Display_ShowString(78, 0, Content, 22, FONTSIZE_8);

    // Need repair
    sprintf(Content, "Device Failure!");
    Display_ShowString(8, 1, Content, 22, FONTSIZE_16);

    // Fan and light state and number of terminal
    if ((Host_Information.HostState & ShowerHost_Fan) == ShowerHost_Fan)
    {
        sprintf(Content, "Fan ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Fan OFF", Host_Information.Humidity);
    }
    Display_ShowString(0, 3, Content, 22, FONTSIZE_8);
    if ((Host_Information.HostState & ShowerHost_Light) == ShowerHost_Light)
    {
        sprintf(Content, "Light ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Light OFF", Host_Information.Humidity);
    }
    Display_ShowString(48, 3, Content, 22, FONTSIZE_8);
    sprintf(Content, "%d", Host_Information.TerminalNumber);
    Display_ShowString(108, 3, Content, 22, FONTSIZE_8);
}

void ShowerHost_DisplayNormal(void)
{
    static uint8_t cnt = 0;
    uint8_t Content[22];

    // Date and time
    sprintf(Content, "%02d%02d%02d %02d:%02d ", Host_Information.CurrentDate.RTC_Year,
            Host_Information.CurrentDate.RTC_Month, Host_Information.CurrentDate.RTC_Date,
            Host_Information.CurrentTime.RTC_Hours, Host_Information.CurrentTime.RTC_Minutes);
    Display_ShowString(0, 0, Content, 22, FONTSIZE_8);

    // Water supply
    if ((Host_Information.HostState & ShowerHost_WaterSupply) == ShowerHost_WaterSupply)
    {
        sprintf(Content, "WaterON");
    }
    else
    {
        sprintf(Content, "NoWater");
    }
    Display_ShowString(78, 0, Content, 22, FONTSIZE_8);

    // Temperature or humidity
    if (cnt % 2 == 0)
    {
        sprintf(Content, "Temp: %.2f  ", Host_Information.Temperature);
    }
    else
    {
        sprintf(Content, "Hum: %.2f   ", Host_Information.Humidity);
    }
    Display_ShowString(24, 1, Content, 22, FONTSIZE_16);

    // Fan and light state and number of terminal
    if ((Host_Information.HostState & ShowerHost_Fan) == ShowerHost_Fan)
    {
        sprintf(Content, "Fan ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Fan OFF", Host_Information.Humidity);
    }
    Display_ShowString(0, 3, Content, 22, FONTSIZE_8);
    if ((Host_Information.HostState & ShowerHost_Light) == ShowerHost_Light)
    {
        sprintf(Content, "Light ON ", Host_Information.Humidity);
    }
    else
    {
        sprintf(Content, "Light OFF", Host_Information.Humidity);
    }
    Display_ShowString(48, 3, Content, 22, FONTSIZE_8);
    sprintf(Content, "%d", Host_Information.TerminalNumber);
    Display_ShowString(108, 3, Content, 22, FONTSIZE_8);

    cnt++;
}
/***********************************END OF FILE********************************/
