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
#define MAXZIGBEELENGTH 256

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
uint8_t OnlineTerminalFlag[TerminalMaxNumber + 1] = {0};
uint8_t Terminal_InformationFromCloud[TerminalMaxNumber + 1][MAXZIGBEELENGTH];
uint8_t Terminal_InformationFromTerminal[TerminalMaxNumber + 1][MAXZIGBEELENGTH];
uint8_t ReceivedOnceFromTerminalFlag[TerminalMaxNumber + 1] = {0};
float LightSensorThreshold = 1.65;
float TemperatureSensorThreshold = 30;
float HumiditySensorThreshold = 50;
uint8_t GetNewDataFromTerminal = 0;
uint8_t GetNewDataFromCloud = 0;

uint8_t ACK[MAXZIGBEELENGTH];
uint8_t NAK[MAXZIGBEELENGTH];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Conver uint32 to uint8.
 * @param  Data: Uint32 data.
 * @param  Data0: Uint8 data0.
 * @param  Data1: Uint8 data1.
 * @param  Data2: Uint8 data2.
 * @param  Data3: Uint8 data3.
 * @retval None.
 */
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

/**
 * @brief  Conver uint8 to uint32.
 * @param  Data0: Uint8 data0.
 * @param  Data1: Uint8 data1.
 * @param  Data2: Uint8 data2.
 * @param  Data3: Uint8 data3.
 * @retval Uint32 data.
 */
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

/**
 * @brief  Conver int32 to uint8.
 * @param  Data: Int32 data.
 * @param  Data0: Uint8 data0.
 * @param  Data1: Uint8 data1.
 * @param  Data2: Uint8 data2.
 * @param  Data3: Uint8 data3.
 * @retval None.
 */
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

/**
 * @brief  Conver uint8 to int32.
 * @param  Data0: Uint8 data0.
 * @param  Data1: Uint8 data1.
 * @param  Data2: Uint8 data2.
 * @param  Data3: Uint8 data3.
 * @retval Int32 data.
 */
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
    uint8_t Content[64];

    // Wait supply stable
    Delay_ms(10);

    // Initialize display
    Display_Init();
    Display_Clear();

    // Initialize date and time recoder
    sprintf(Content, "RTC Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    Information_InitializeRTC();

    // Initialize information
    Display_Clear();
    sprintf(Content, "Setting information...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    ShowerHost_InitInformation();

    // Initialize audio
    Display_Clear();
    sprintf(Content, "Audio Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    // Audio_Init();
    Audio_Volume(15);

    // Initialize buttons
    Display_Clear();
    sprintf(Content, "Button Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    Button_Init();

    // Initialize communicate
    Display_Clear();
    sprintf(Content, "Communication Device Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    Communicate_Init(ShowerHostSerial, Communicate_ZigBeeChannel,
                     Communicate_ZigBeePANID, Communicate_ZigBeeGroupID);

    // Initialize light sensor
    Display_Clear();
    sprintf(Content, "Light Sensor Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    LightSensor_Init(LightSensorThreshold);

    // Initialize signal output
    Display_Clear();
    sprintf(Content, "Fan and Light Controller Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    SignalOutput_Init();

    // Initialize temperature - humdity sensor
    Display_Clear();
    sprintf(Content, "Temperature - Humidity Sensor Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    TemperatureHumiditySensor_Init(TemperatureSensorThreshold, HumiditySensorThreshold);

    // Display initialization finished
    Display_Clear();
    sprintf(Content, "Initialized!");
    Display_ShowString(16, 1, Content, 0xFF, FONTSIZE_16);
    Delay_s(5);
}

/**
 * @brief  Check acknowledge from destination.
 * @param  None.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerHost_ZigBeeCheck(uint8_t Serial)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    for (int i = 0; i < 50; i++)
    {
        Delay_ms(2);
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

/**
 * @brief  Check acknowledge from destination.
 * @param  None.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerHost_WiFiCheck(void)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    for (int i = 0; i < 50; i++)
    {
        Delay_ms(2);
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
 * @brief  Send information of terminal to terminal by ZigBee.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure, 2 for not send.
 */
uint8_t ShowerHost_SendZigBeeData(uint8_t Timeout)
{
    if ((GetNewDataFromTerminal == 1) && (GetNewDataFromCloud == 1))
    {
        uint8_t status = 0;
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
                Delay_ms(100);
                Communicate_ZigBeeTX(ZigBee_TypeRouter, Terminal_InformationFromCloud[i][1], TransmitData, 32);
                status = ShowerHost_ZigBeeCheck(Terminal_InformationFromCloud[i][1]);
                if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
                    return status;
            } while (status == 0); // Check ACK
        }
        GetNewDataFromTerminal = 0;
        GetNewDataFromCloud = 0;
        return status;
    }
    return 2;
}

/**
 * @brief  Send information of host and terminal to cloud by WiFi.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerHost_SendWiFiData(uint8_t Timeout)
{
    uint8_t status = 0;
    uint8_t TransmitData[MAXZIGBEELENGTH];
    uint32_t StartTime = Information_GetTimeStamp();

    TransmitData[0] = '$';
    TransmitData[1] = ShowerHostSerial;
    TransmitData[2] = Host_Information.HostState;
    Int32toInt8((int32_t)(Host_Information.Temperature * 100.0), &TransmitData[3], &TransmitData[4],
                &TransmitData[5], &TransmitData[6]);
    Int32toInt8((int32_t)(Host_Information.Humidity * 100.0), &TransmitData[7], &TransmitData[8],
                &TransmitData[9], &TransmitData[10]);
    Int32toInt8((int32_t)(Host_Information.Light * 100.0), &TransmitData[11], &TransmitData[12],
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

    do
    {
        Delay_ms(100);
        Communicate_WiFiTX(TransmitData, 19 + Host_Information.TerminalNumber * 24);
        status = ShowerHost_WiFiCheck();
        if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
            return status;
    } while (status == 0);
}

/**
 * @brief  Get data from cloud server and resolve.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerHost_GetWiFiData(uint8_t Timeout)
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
                        memcpy(Terminal_InformationFromCloud[CurrentTerminalSerial],
                               &ReceivedData[13 + i * 32], 32);
                    }

                    // ACK
                    sprintf(ACK, "$%cA$", ShowerHostSerial);
                    Communicate_WiFiTX(ACK, 4);
                    ShowerHost_SetDevice();
                    correct = 1;
                    GetNewDataFromCloud = 1;
                }
            }
            else // Received but error
            {
                sprintf(NAK, "$%cN$", ShowerHostSerial);
                Communicate_WiFiTX(NAK, 4);
            }
        }
        if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
            return correct;
    } while ((length == 0) || (correct != 1));

    return correct;
}

/**
 * @brief  Get data from terminal.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerHost_GetZigBeeData(uint8_t Timeout)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    uint8_t CurrentTerminalNumber = 0;
    uint8_t status = 0;

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
                        for (int j = 0; j < 5; j++)
                        {
                            Delay_ms(100);
                            Communicate_ZigBeeTX(ZigBee_TypeRouter, CurrentTerminalSerial, ACK, 4);
                        }

                        correct = 1;
                        status = 1;
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

    return status;
}

/**
 * @brief  Set host according to information.
 * @param  None.
 * @retval None.
 */
void ShowerHost_SetDevice(void)
{
    // Date & time
    Information_SetDateTime(&Host_Information.CurrentDate, &Host_Information.CurrentTime);

    // Fan
    // Set by cloud
    if ((Host_Information.StateControl & ShowerHost_Fan) == ShowerHost_Fan)
    {
        if ((Host_Information.HostState & ShowerHost_Fan) == ShowerHost_Fan)
        {
            if (Fan_GetState() == 0)
            {
                Audio_Play(Audio_Remind);
                Delay_ms(10);
            }
            Fan_Start();
        }
        else
        {
            if (Fan_GetState() == 1)
            {
                Audio_Play(Audio_Remind);
                Delay_ms(10);
            }
            Fan_Stop();
        }
    }

    // Light
    // Set by cloud
    if ((Host_Information.StateControl & ShowerHost_Light) == ShowerHost_Light)
    {
        if ((Host_Information.HostState & ShowerHost_Light) == ShowerHost_Light)
        {
            if (Light_GetState() == 0)
            {
                Audio_Play(Audio_Remind);
                Delay_ms(10);
            }
            Light_Start();
        }
        else
        {
            if (Light_GetState() == 1)
            {
                Audio_Play(Audio_Remind);
                Delay_ms(10);
            }
            Light_Stop();
        }
    }
}

/**
 * @brief  Check whether need repair.
 * @param  None.
 * @retval Status, 1 for need, 0 for not.
 */
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

/**
 * @brief  Check whether need help.
 * @param  None.
 * @retval Status, 1 for need, 0 for not.
 */
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

/**
 * @brief  Get temperature.
 * @param  None.
 * @retval None.
 */
void ShowerHost_TemperatureDetect(void)
{
    Sensor_Convert();
    Host_Information.Temperature = Sensor_GetTemperature();
}

/**
 * @brief  Get humidity.
 * @param  None.
 * @retval None.
 */
void ShowerHost_HumidityDetect(void)
{
    Sensor_Convert();
    Host_Information.Humidity = Sensor_GetHumidity();
}

/**
 * @brief  Get light intensity.
 * @param  None.
 * @retval None.
 */
void ShowerHost_LightDetect(void)
{
    Host_Information.Light = LightSensor_GetValue();
}

/**
 * @brief  Set repair state to host state.
 * @param  None.
 * @retval None.
 */
void ShowerHost_SetRepairState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Repair;
}

/**
 * @brief  Set help state to host state.
 * @param  None.
 * @retval None.
 */
void ShowerHost_SetHelpState(void)
{
    Host_Information.HostState = Host_Information.HostState | ShowerHost_Help;
}

/**
 * @brief  Control fan according to temperature, humidity and cloud server.
 * @param  None.
 * @retval None.
 */
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
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Fan_Start();
        Host_Information.HostState = Host_Information.HostState | ShowerHost_Fan;
    }
    else
    {
        if (Fan_GetState() == 1)
        {
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Fan_Stop();
        Host_Information.HostState = Host_Information.HostState & (~ShowerHost_Fan);
    }
}

/**
 * @brief  Control light according to time, light intensity and cloud server.
 * @param  None.
 * @retval None.
 */
void ShowerHost_LightControl(void)
{
    // Set by cloud
    if ((Host_Information.StateControl & ShowerHost_Light) == ShowerHost_Light)
    {
        return;
    }

    // Only turn on when night, when not controlled by cloud
    if ((Host_Information.CurrentTime.RTC_Hours >= 8) && (Host_Information.CurrentTime.RTC_Hours <= 18))
    {
        if (Light_GetState() == 1)
        {
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Light_Stop();
        Host_Information.HostState = Host_Information.HostState & (~ShowerHost_Light);
        return;
    }

    if (LightSensor_GetState())
    {
        if (Light_GetState() == 0)
        {
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Light_Start();
        Host_Information.HostState = Host_Information.HostState | ShowerHost_Light;
    }
    else
    {
        if (Light_GetState() == 1)
        {
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Light_Stop();
        Host_Information.HostState = Host_Information.HostState & (~ShowerHost_Light);
    }
}

/**
 * @brief  Display when in need of help.
 * @param  None.
 * @retval None.
 */
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

/**
 * @brief  Display when in need of repair.
 * @param  None.
 * @retval None.
 */
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

/**
 * @brief  Display normal content.
 * @param  None.
 * @retval None.
 */
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
