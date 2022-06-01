/**
 ******************************************************************************
 * @file    UserLibrary.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.6.1
 * @brief   This file contains all the functions for user libraries.(For shower
 *          terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "UserLibrary.h"
#include "string.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAXZIGBEELENGTH 128

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Terminal information
uint8_t ShowerTerminalSerial = 0x01;
uint8_t FatherHostSerial = 0x01;
ShowerTerminal_InformationTypeDef Terminal_Information;
uint8_t CardCurrentState = 0;
float price = 0.04;
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
 * @brief  Initialize shower terminal information.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_InitInformation(void)
{
    Terminal_Information.HostSerial = FatherHostSerial;
    Terminal_Information.TerminalSerial = ShowerTerminalSerial;
    Terminal_Information.NFCSerial = 0;
    Terminal_Information.UserSerial = 0;
    Terminal_Information.ReservedUser = 0;
    Information_GetDateTime(&Terminal_Information.CurrentDate, &Terminal_Information.CurrentTime);
    Terminal_Information.WaterTemperature = 0;
    Terminal_Information.WaterFlow = 0;
    Terminal_Information.AccountBalance = 0;
    Terminal_Information.TerminalState = 0xC0;
}

/**
 * @brief  Initialize all peripherals of shower terminal.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_Init(void)
{
    // Wait supply stable
    Delay_ms(10);

    // Initialize date and time recoder
    Information_InitializeRTC();

    // Initialize information
    ShowerTerminal_InitInformation();

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

    // Initialize flow meter
    FlowMeter_Init();
    FlowMeter_Clear();

    // Initialize infrared human sensor
    InfraredHuman_Init();

    // Initialize infrared object sensor
    InfraredObject_Init();

    // Initialize NFC reader
    NFC_Init();

    // Initialize temperature sensor
    TemperatureSensor_Init();

    // Initialize valve control
    Valve_Init();
    Valve_Stop();

    Delay_s(2);

    // First upload
    // ShowerTerminal_SendZigBeeData();

    // Display initialization finished
    uint8_t Content[] = {"Initialized!"};
    Display_ShowString(16, 1, Content, 0xFF, FONTSIZE_16);
    Delay_s(1);
    Display_Clear();
}

/**
 * @brief  Get data from host and resolve.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_GetZigBeeData(void)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    uint16_t length;
    uint8_t correct = 0;
    uint32_t StartTime = Information_GetTimeStamp();
    do
    {
        length = Communicate_ZigBeeRX(ReceivedData);
        if (length != 0)
        {
            if ((ReceivedData[0] == '$') && (ReceivedData[30] == '$')) // Whole frame
            {
                Terminal_Information.UserSerial = Int8toUint32(ReceivedData[1], ReceivedData[2], ReceivedData[3], ReceivedData[4]);
                Terminal_Information.TerminalState = ReceivedData[5];
                Terminal_Information.AccountBalance = (float)Int8toInt32(ReceivedData[6], ReceivedData[7], ReceivedData[8], ReceivedData[9]) / 100;

                Terminal_Information.CurrentDate.RTC_Year = ReceivedData[10];
                Terminal_Information.CurrentDate.RTC_Month = ReceivedData[11];
                Terminal_Information.CurrentDate.RTC_Date = ReceivedData[12];
                Terminal_Information.CurrentDate.RTC_WeekDay = ReceivedData[13];

                Terminal_Information.CurrentTime.RTC_Hours = ReceivedData[14];
                Terminal_Information.CurrentTime.RTC_Minutes = ReceivedData[15];
                Terminal_Information.CurrentTime.RTC_Seconds = ReceivedData[16];
                Terminal_Information.CurrentTime.RTC_H12 = ReceivedData[17];

                Terminal_Information.ReserveStartTime.RTC_Hours = ReceivedData[18];
                Terminal_Information.ReserveStartTime.RTC_Minutes = ReceivedData[19];
                Terminal_Information.ReserveStartTime.RTC_Seconds = ReceivedData[20];
                Terminal_Information.ReserveStartTime.RTC_H12 = ReceivedData[21];

                Terminal_Information.ReserveStopTime.RTC_Hours = ReceivedData[22];
                Terminal_Information.ReserveStopTime.RTC_Minutes = ReceivedData[23];
                Terminal_Information.ReserveStopTime.RTC_Seconds = ReceivedData[24];
                Terminal_Information.ReserveStopTime.RTC_H12 = ReceivedData[25];

                Terminal_Information.ReservedUser = Int8toUint32(ReceivedData[26], ReceivedData[27], ReceivedData[28], ReceivedData[29]);

                sprintf(ACK, "$%cA$", ShowerTerminalSerial);
                Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, ACK, 4);
                correct = 1;
            }
            else // Received but error
            {
                sprintf(NAK, "$%cN$", ShowerTerminalSerial);
                Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, NAK, 4);
            }
        }
        if ((Information_GetTimeStamp() - StartTime) > 5)
            return;
    } while ((length == 0) || (correct != 1));
}

void ShowerTerminal_SetDevice(void)
{
    // Date & time
    Information_SetDateTime(&Terminal_Information.CurrentDate, &Terminal_Information.CurrentTime);
}

uint8_t ShowerTerminal_ObjectLeftCheck(void)
{
    if ((Terminal_Information.TerminalState & ShowerTerminal_ObjectLeft) == ShowerTerminal_ObjectLeft)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_GetRepairState(void)
{
    if ((Terminal_Information.TerminalState & ShowerTerminal_Repair) == ShowerTerminal_Repair)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_GetHelpState(void)
{
    if ((Terminal_Information.TerminalState & ShowerTerminal_Help) == ShowerTerminal_Help)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_HumanDetect(void)
{
    if (InfraredHuman_GetState())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_ObjectDetect(void)
{
    if (InfraredObject_GetState())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_CardDetect(void)
{
    uint8_t key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t tmp[16];
    if (NFC_ReadData(1, 0, key, tmp) == (char)0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_CardFirstRead(void)
{
    if (CardCurrentState == 0)
    {
        CardCurrentState = 1;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_CardTake(void)
{
    if (CardCurrentState == 1)
    {
        CardCurrentState = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ShowerTerminal_ReservationCheck(void)
{
    // Reserved and current user matches
    if (((Terminal_Information.TerminalState &
          ShowerTerminal_Reserve) == ShowerTerminal_Reserve) &&
        (Terminal_Information.UserSerial == Terminal_Information.ReservedUser))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ShowerTerminal_TakeObjectProcess(void)
{
    if (ShowerTerminal_CardDetect() || ShowerTerminal_ObjectDetect())
    {
        ShowerTerminal_CardObjectLeft();
    }
    else
    {
        Terminal_Information.TerminalState &= (~ShowerTerminal_ObjectLeft);
        ShowerTerminal_IdleState();
    }
}

void ShowerTerminal_SetHelpState(void)
{
    Terminal_Information.TerminalState = Terminal_Information.TerminalState | ShowerTerminal_Help;
}

void ShowerTerminal_SetRepairState(void)
{
    Terminal_Information.TerminalState = Terminal_Information.TerminalState | ShowerTerminal_Repair;
}

void ShowerTerminal_CardFirstReadProcess(void)
{
    // Audio and display
    Audio_Play(AudioRemind);
    Delay_ms(10);
    uint8_t Content[] = "Wait...";
    ShowerTerminal_DisplayContent(Content);

    // Get card and user information
    Terminal_Information.NFCSerial = NFC_SetCheckCardNumber(0, NFC_CHECK);
    Terminal_Information.UserSerial = NFC_SetCheckUserNumber(0, NFC_CHECK);
    Terminal_Information.TerminalState |= ShowerTerminal_FirstReadCard; // Set flag

    // Query data
    ShowerTerminal_SendZigBeeData();
    ShowerTerminal_GetZigBeeData();
    ShowerTerminal_SetDevice();

    // Clear flag
    Terminal_Information.TerminalState &= (~ShowerTerminal_FirstReadCard);

    // Preparation
    if (ShowerTerminal_ReservationCheck())
    {
        // Valve on
        if (Valve_GetState() == 0)
        {
            Audio_Play(AudioRemind);
            Delay_ms(10);
        }
        Valve_Start();

        // Flow meter on
        FlowMeter_Clear();
        FlowMeter_Start();
    }
}

void ShowerTerminal_InUseProcess(void)
{
    // Set flag
    Terminal_Information.TerminalState |= ShowerTerminal_InUse;

    // Get temperature
    Sensor_Convert();
    Terminal_Information.WaterTemperature = Sensor_GetTemperature();

    // Get flow
    Terminal_Information.WaterFlow = FlowMeter_GetValueL();
    float remain = Terminal_Information.AccountBalance - Terminal_Information.WaterFlow * price;

    // Display
    static uint8_t cnt = 0;
    uint8_t Content[17];
    if (cnt % 3 == 0)
    {
        sprintf(Content, "Temp: %.2f", Terminal_Information.WaterTemperature);
    }
    else if (cnt % 3 == 1)
    {
        sprintf(Content, "Flow: %.2f", Terminal_Information.WaterFlow);
    }
    else
    {
        sprintf(Content, "Remain: %.2f", remain);
    }
    ShowerTerminal_DisplayContent(Content);
    cnt++;
    Delay_s(1);
}

void ShowerTerminal_WaterStopRemind(void)
{
    uint8_t Content[17];
    if ((Terminal_Information.TerminalState & ShowerTerminal_WaterSupply) != ShowerTerminal_WaterSupply)
    {
        Audio_Play(AudioWarn);
        sprintf(Content, "Water Stop!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(3);
    }
}

void ShowerTerminal_NearReservationEnd(void)
{
    static uint8_t cnt = 0;
    uint8_t Content[17];
    uint32_t ReserveEndTime = Terminal_Information.ReserveStopTime.RTC_Hours * 3600 +
                              Terminal_Information.ReserveStopTime.RTC_Minutes * 60 +
                              Terminal_Information.ReserveStopTime.RTC_Seconds;
    uint32_t CurrentTime = Terminal_Information.CurrentTime.RTC_Hours * 3600 +
                           Terminal_Information.CurrentTime.RTC_Minutes * 60 +
                           Terminal_Information.CurrentTime.RTC_Seconds;
    if (((cnt % 5) == 0) && (CurrentTime >= ReserveEndTime))
    {
        Audio_Play(AudioWarn);
        sprintf(Content, "Time Up!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(3);
    }
    if (((cnt % 15) == 0) && (ReserveEndTime - CurrentTime) <= 300)
    {
        Audio_Play(AudioWarn);
        sprintf(Content, "Time Up Soon!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(3);
    }
    cnt++;
}

void ShowerTerminal_NoReservationProcess(void)
{
    uint8_t Content[17];
    Audio_Play(AudioRemind);
    sprintf(Content, "No Appointment!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(1);
}

void ShowerTerminal_CardTakeProcess(void)
{
    uint8_t Content[17];

    // Check if in use
    if ((Terminal_Information.TerminalState & ShowerTerminal_InUse) != ShowerTerminal_InUse)
    {
        // Clear information
        Terminal_Information.NFCSerial = 0;
        Terminal_Information.UserSerial = 0;
        Terminal_Information.WaterTemperature = 0;
        Terminal_Information.WaterFlow = 0;
        Terminal_Information.AccountBalance = 0;
        return;
    }

    // Flow stop
    Terminal_Information.WaterFlow = FlowMeter_GetValueL();
    float cost = Terminal_Information.WaterFlow * price;
    Terminal_Information.AccountBalance -= cost;
    FlowMeter_Stop();
    FlowMeter_Clear();

    // Valve stop
    if (Valve_GetState() == 1)
    {
        Audio_Play(AudioRemind);
        Delay_ms(10);
    }
    Valve_Stop();

    // Display
    sprintf(Content, "Cost: %.2f", cost);
    ShowerTerminal_DisplayContent(Content);

    // Upload
    Terminal_Information.TerminalState |= ShowerTerminal_CardOff;
    ShowerTerminal_SendZigBeeData();
    ShowerTerminal_GetZigBeeData();
    ShowerTerminal_SetDevice();
    Terminal_Information.TerminalState &= (~ShowerTerminal_CardOff);

    // Clear flag
    Terminal_Information.TerminalState &= (~ShowerTerminal_InUse);

    // Clear information
    Terminal_Information.NFCSerial = 0;
    Terminal_Information.UserSerial = 0;
    Terminal_Information.WaterTemperature = 0;
    Terminal_Information.WaterFlow = 0;
    Terminal_Information.AccountBalance = 0;
}

void ShowerTerminal_NotUseProcess(void)
{
    uint8_t Content[17];
    Audio_Play(AudioRemind);
    sprintf(Content, "Swipe Card!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(1);
}

void ShowerTerminal_CardObjectLeft(void)
{
    // Set flag
    Terminal_Information.TerminalState |= ShowerTerminal_ObjectLeft;

    uint8_t Content[17];
    Audio_Play(AudioWarn);
    sprintf(Content, "Object Left!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(1);
}

void ShowerTerminal_IdleState(void)
{
    uint8_t Content[17];
    if ((Terminal_Information.TerminalState & ShowerTerminal_Reserve) == ShowerTerminal_Reserve)
    {
        sprintf(Content, "Reserved!");
    }
    else
    {
        sprintf(Content, "Idle...");
    }
    ShowerTerminal_DisplayContent(Content);
    Delay_s(1);
}

uint8_t ShowerTerminal_ZigBeeCheck(void)
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
        else if (ReceivedData[1] != ShowerTerminalSerial)
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

void ShowerTerminal_SendZigBeeData(void)
{
    uint8_t TransmitData[MAXZIGBEELENGTH];
    TransmitData[0] = '$';
    TransmitData[1] = ShowerTerminalSerial;
    Uint32toInt8(Terminal_Information.NFCSerial, &TransmitData[2], &TransmitData[3],
                 &TransmitData[4], &TransmitData[5]);
    Uint32toInt8(Terminal_Information.UserSerial, &TransmitData[6], &TransmitData[7],
                 &TransmitData[8], &TransmitData[9]);
    TransmitData[10] = Terminal_Information.TerminalState;
    Int32toInt8((int32_t)Terminal_Information.WaterTemperature * 100, &TransmitData[11],
                &TransmitData[12], &TransmitData[13], &TransmitData[14]);
    Int32toInt8((int32_t)Terminal_Information.WaterFlow * 100, &TransmitData[15],
                &TransmitData[16], &TransmitData[17], &TransmitData[18]);
    Int32toInt8((int32_t)Terminal_Information.AccountBalance * 100, &TransmitData[19],
                &TransmitData[20], &TransmitData[21], &TransmitData[22]);
    TransmitData[23] = '$';

    Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, TransmitData, 24);

    // Check
    if (ShowerTerminal_ZigBeeCheck() == 0)
    {
        Delay_ms(5);
        Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, TransmitData, 24);
    }
}

void ShowerTerminal_DisplayContent(uint8_t *MainContent)
{
    uint8_t Content[22];

    // Date and time
    sprintf(Content, "%02d%02d%02d %02d:%02d ", Terminal_Information.CurrentDate.RTC_Year,
            Terminal_Information.CurrentDate.RTC_Month, Terminal_Information.CurrentDate.RTC_Date,
            Terminal_Information.CurrentTime.RTC_Hours, Terminal_Information.CurrentTime.RTC_Minutes);
    Display_ShowString(0, 0, Content, 22, FONTSIZE_8);

    // Water supply
    if ((Terminal_Information.TerminalState & ShowerTerminal_WaterSupply) == ShowerTerminal_WaterSupply)
    {
        sprintf(Content, "WaterON");
    }
    else
    {
        sprintf(Content, "NoWater");
    }
    Display_ShowString(78, 0, Content, 22, FONTSIZE_8);

    // Main content
    uint8_t FrontSpace = (16 - strlen(MainContent)) / 2;
    for (int i = 0; i < FrontSpace; i++)
    {
        Display_ShowChar(i * 8, 1, ' ', FONTSIZE_16);
    }
    Display_ShowString(FrontSpace * 8, 1, MainContent, strlen(MainContent), FONTSIZE_16);
    for (int i = FrontSpace + strlen(MainContent); i < 16; i++)
    {
        Display_ShowChar(i * 8, 1, ' ', FONTSIZE_16);
    }

    // Valve state and number of terminal
    if (Valve_GetState())
    {
        sprintf(Content, "Valve ON ");
    }
    else
    {
        sprintf(Content, "Valve OFF ");
    }
    Display_ShowString(0, 3, Content, 22, FONTSIZE_8);

    sprintf(Content, "%011d", Terminal_Information.UserSerial);
    Display_ShowString(60, 3, Content, 22, FONTSIZE_8);
}
/***********************************END OF FILE********************************/
