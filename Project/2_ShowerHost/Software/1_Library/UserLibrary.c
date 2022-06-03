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
#include "string.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TerminalMaxNumber 6
#define MAXWIFIDTALENGTH 1024
#define MAXZIGBEELENGTH 1024

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Host information
uint8_t ShowerHostSerial = 0x01;
uint8_t ShowerTerminalNumber = 0;
uint8_t Communicate_ZigBeeChannel = 15;
uint16_t Communicate_ZigBeePANID = 0x1234;
uint16_t Communicate_ZigBeeGroupID = 0x1111;

ShowerHost_InformationTypeDef Host_Information;

// Terminal information buffer
uint8_t Terminal_InformationFromCloud[TerminalMaxNumber + 1][MAXZIGBEELENGTH];
uint8_t OnlineTerminalFlag[TerminalMaxNumber + 1] = {0};
uint8_t ReceivedOnceFromTerminalFlag[TerminalMaxNumber + 1] = {0};
uint8_t Terminal_InformationFromTerminal[TerminalMaxNumber + 1][MAXZIGBEELENGTH];
float LightSensorThreshold = 2.8;
float TemperatureSensorThreshold = 26;
float HumiditySensorThreshold = 30;
uint8_t GetNewDataFromTerminal = 0;
uint8_t GetNewDataFromCloud = 0;

uint8_t ACK[MAXZIGBEELENGTH];
uint8_t NAK[MAXZIGBEELENGTH];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Uint32toInt8(uint32_t Data, uint8_t *Data0, uint8_t *Data1, uint8_t *Data2, uint8_t *Data3)
{
    *Data0 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data1 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data2 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data3 = Data & 0x000000FF;
}

uint32_t Int8toUint32(uint8_t Data0, uint8_t Data1, uint8_t Data2, uint8_t Data3)
{
    uint32_t temp = 0;
    temp = temp | Data3;
    temp = temp << 8;
    temp = temp | Data2;
    temp = temp << 8;
    temp = temp | Data1;
    temp = temp << 8;
    temp = temp | Data0;
    return temp;
}

void Int32toInt8(int32_t Data, uint8_t *Data0, uint8_t *Data1, uint8_t *Data2, uint8_t *Data3)
{
    *Data0 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data1 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data2 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data3 = Data & 0x000000FF;
}

int32_t Int8toInt32(uint8_t Data0, uint8_t Data1, uint8_t Data2, uint8_t Data3)
{
    int32_t temp = 0;
    temp = temp | Data3;
    temp = temp << 8;
    temp = temp | Data2;
    temp = temp << 8;
    temp = temp | Data1;
    temp = temp << 8;
    temp = temp | Data0;
    return temp;
}

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
    Communicate_Init(ShowerHostSerial, Communicate_ZigBeeChannel,
                     Communicate_ZigBeePANID, Communicate_ZigBeeGroupID);

    // Initialize display
    Display_Init();
    Display_Clear();

    // Initialize light sensor
    LightSensor_Init(LightSensorThreshold);

    // Initialize signal output
    SignalOutput_Init();

    // Initialize temperature - humdity sensor
    TemperatureHumiditySensor_Init(TemperatureSensorThreshold, HumiditySensorThreshold);

    // Display initialization finished
    uint8_t Content[] = {"Initialized!"};
    Display_ShowString(16, 1, Content, 0xFF, FONTSIZE_16);
    Delay_s(10);
    Display_Clear();
}

uint8_t ShowerHost_ZigBeeCheck(uint8_t Serial)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    for (int i = 0; i < 10; i++)
    {
        Delay_ms(1);
        if (Communicate_ZigBeeRX(ReceivedData) == 0)
        {
            continue;
        }
        else if ((ReceivedData[0] != '$') || (ReceivedData[3] != '$'))
        {
            continue;
        }
        else if (ReceivedData[1] != Serial)
        {
            continue;
        }
        else if (ReceivedData[2] == 'A')
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

uint8_t ShowerHost_WiFiCheck(void)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    for (int i = 0; i < 10; i++)
    {
        Delay_ms(1);
        if (Communicate_WiFiRX(ReceivedData) == 0)
        {
            continue;
        }
        else if ((ReceivedData[0] != '$') || (ReceivedData[3] != '$'))
        {
            continue;
        }
        else if (ReceivedData[1] != ShowerHostSerial)
        {
            continue;
        }
        else if (ReceivedData[2] == 'A')
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

/**
 * @brief  Get data from cloud server and resolve.
 * @param  None.
 * @retval None.
 */
void ShowerHost_GetWiFiData(uint8_t Timeout)
{
    uint8_t ReceivedData[MAXWIFIDTALENGTH];
    uint16_t length = 0;
    uint8_t correct = 0;
    uint32_t StartTime = Information_GetTimeStamp();
    do
    {
        length = Communicate_WiFiRX(ReceivedData);
        if (length != 0)
        {
            if ((ReceivedData[0] == '$') && (ReceivedData[length - 3] == '$')) // Whole frame
            {
                if (ReceivedData[1] == Host_Information.HostSerial) // Check address
                {
                    Host_Information.HostState = ReceivedData[2];
                    Host_Information.StateControl = ReceivedData[3];

                    Host_Information.CurrentDate.RTC_Year = ReceivedData[4];
                    Host_Information.CurrentDate.RTC_Month = ReceivedData[5];
                    Host_Information.CurrentDate.RTC_Date = ReceivedData[6];
                    Host_Information.CurrentDate.RTC_WeekDay = ReceivedData[7];

                    Host_Information.CurrentTime.RTC_Hours = ReceivedData[8];
                    Host_Information.CurrentTime.RTC_Minutes = ReceivedData[9];
                    Host_Information.CurrentTime.RTC_Seconds = ReceivedData[10];
                    Host_Information.CurrentTime.RTC_H12 = ReceivedData[11];

                    uint8_t num = ReceivedData[12];
                    for (int i = 0; i < num; i++)
                    {
                        uint8_t CurrentTerminalSerial = ReceivedData[14 + i * 32];
                        for (int j = 0; j < 32; j++)
                        {
                            Terminal_InformationFromCloud[CurrentTerminalSerial][j] = ReceivedData[13 + i * 32 + j];
                        }
                    }
                    sprintf(ACK, "$%cA$", ShowerHostSerial);
                    Communicate_WiFiTX(ACK, 4);
                    correct = 1;
                    GetNewDataFromCloud = 1;
                    ShowerHost_SetDevice();
                }
            }
            else // Received but error
            {
                sprintf(NAK, "$%cN$", ShowerHostSerial);
                Communicate_WiFiTX(NAK, 4);
            }
        }
        if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
            return;
    } while ((length == 0) || (correct != 1));
}

void ShowerHost_GetZigBeeData(uint8_t Timeout)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    uint8_t CurrentTerminalNumber = 0;
    for (int i = 0; i < Host_Information.TerminalNumber + 1; i++) // Try to check and add 1 new terminal each time
    {
        uint16_t length = 0;
        uint8_t CurrentTerminalSerial;
        uint32_t StartTime = Information_GetTimeStamp();
        uint8_t correct = 0;

        do
        {
            length = Communicate_ZigBeeRX(ReceivedData);
            if (length != 0)
            {
                if ((ReceivedData[0] == '$') && (ReceivedData[23] == '$')) // Whole frame
                {
                    CurrentTerminalSerial = ReceivedData[1];
                    if (ReceivedOnceFromTerminalFlag[CurrentTerminalSerial] == 0)
                    {
                        ReceivedOnceFromTerminalFlag[CurrentTerminalSerial] == 1; // Avoid repeat
                        memcpy(Terminal_InformationFromTerminal[CurrentTerminalSerial], ReceivedData, 24);
                        OnlineTerminalFlag[CurrentTerminalSerial] = 1; // Add terminal
                        sprintf(ACK, "$%cA$", CurrentTerminalSerial);
                        Communicate_ZigBeeTX(ZigBee_TypeRouter, CurrentTerminalSerial, ACK, 4);
                        correct = 1;
                        GetNewDataFromTerminal = 1;
                    }
                }
            }
            if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
                break;
        } while ((length == 0) || (correct != 1));
    }

    memset(ReceivedOnceFromTerminalFlag, 0, sizeof(ReceivedOnceFromTerminalFlag));
    for (int i = 1; i < TerminalMaxNumber + 1; i++)
    {
        CurrentTerminalNumber += OnlineTerminalFlag[i];
    }
    Host_Information.TerminalNumber = CurrentTerminalNumber;
}

void ShowerHost_SendWiFiData(uint8_t Timeout)
{
    uint8_t TransmitData[MAXZIGBEELENGTH];
    TransmitData[0] = '$';
    TransmitData[1] = ShowerHostSerial;
    TransmitData[2] = Host_Information.HostState;
    Int32toInt8((int32_t)Host_Information.Temperature * 100, &TransmitData[3], &TransmitData[4],
                &TransmitData[5], &TransmitData[6]);
    Int32toInt8((int32_t)Host_Information.Humidity * 100, &TransmitData[7], &TransmitData[8],
                &TransmitData[9], &TransmitData[10]);
    Int32toInt8((int32_t)Host_Information.Light * 100, &TransmitData[11], &TransmitData[12],
                &TransmitData[13], &TransmitData[14]);
    TransmitData[15] = Host_Information.TerminalNumber;

    uint8_t cnt = 0;
    for (int i = 0; i < TerminalMaxNumber; i++)
    {
        if (OnlineTerminalFlag[i] == 0)
            continue;
        memcpy(&TransmitData[16 + cnt * 24], Terminal_InformationFromTerminal[i], 24);
        cnt++;
    }
    TransmitData[16 + Host_Information.TerminalNumber * 24] = '$';
    TransmitData[17 + Host_Information.TerminalNumber * 24] = '\r';
    TransmitData[18 + Host_Information.TerminalNumber * 24] = '\n';

    Communicate_WiFiTX(TransmitData, 19 + Host_Information.TerminalNumber * 24);

    // Check
    if (ShowerHost_WiFiCheck() == 0)
    {
        Delay_ms(5);
        Communicate_WiFiTX(TransmitData, 19 + Host_Information.TerminalNumber * 24);
    }
}

void ShowerHost_SendZigBeeData(uint8_t Timeout)
{
    if ((GetNewDataFromTerminal == 1) && (GetNewDataFromCloud == 1))
    {
        uint8_t TransmitData[MAXZIGBEELENGTH];
        uint32_t StartTime = Information_GetTimeStamp();

        for (int i = 0; i < TerminalMaxNumber; i++)
        {
            if (OnlineTerminalFlag[i] == 0)
                continue;

            memcpy(TransmitData, Terminal_InformationFromCloud[i], 32);
            Communicate_ZigBeeTX(ZigBee_TypeRouter, Terminal_InformationFromCloud[i][1], TransmitData, 32);

            do
            {
                Delay_ms(5);
                Communicate_ZigBeeTX(ZigBee_TypeRouter, Terminal_InformationFromCloud[i][1], TransmitData, 32);
                if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
                    return;
            } while (ShowerHost_ZigBeeCheck(Terminal_InformationFromCloud[i][1]) == 0); // Check ACK
        }
        GetNewDataFromTerminal = 0;
        GetNewDataFromCloud = 0;
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

void ShowerHost_LightDetect(void)
{
    Host_Information.Light = LightSensor_GetValue();
}

void ShowerHost_SetRepairState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Repair;
}

void ShowerHost_SetHelpState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Help;
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

void ShowerHost_DisplayHelp(void)
{
    uint8_t Content[22];

    // Date and time
    Information_GetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);
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
    sprintf(Content, "   ");
    Display_ShowString(0, 1, Content, 3, FONTSIZE_16);
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
    Information_GetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);
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
    sprintf(Content, "  ");
    Display_ShowString(0, 1, Content, 2, FONTSIZE_16);
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
    Information_GetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);
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
    sprintf(Content, "   ");
    Display_ShowString(0, 1, Content, 3, FONTSIZE_16);
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
