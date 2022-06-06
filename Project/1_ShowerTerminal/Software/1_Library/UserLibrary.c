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
#define MAXZIGBEELENGTH 256

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Terminal information
uint8_t ShowerTerminalSerial = 0x01;
uint8_t FatherHostSerial = 0x01;
uint8_t Communicate_ZigBeeChannel = 15;
uint16_t Communicate_ZigBeePANID = 0x1234;
uint16_t Communicate_ZigBeeGroupID = 0x1111;

ShowerTerminal_InformationTypeDef Terminal_Information;
uint8_t CardCurrentState = 0;
float price = 0.04;
uint8_t ACK[MAXZIGBEELENGTH];
uint8_t NAK[MAXZIGBEELENGTH];
uint16_t InfraredObject_Threshold = 2048;
uint8_t InfraredHuman_Threshold = 8;
uint8_t QueryFail = 0;

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
    Terminal_Information.TerminalState = 0x00;
}

/**
 * @brief  Initialize all peripherals of shower terminal.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_Init(void)
{
    uint8_t Content[64];

    // Wait poewr supply stable
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
    ShowerTerminal_InitInformation();

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
    Communicate_Init(ShowerTerminalSerial, Communicate_ZigBeeChannel,
                     Communicate_ZigBeePANID, Communicate_ZigBeeGroupID);

    // Initialize flow meter
    Display_Clear();
    sprintf(Content, "Flow Meter Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    FlowMeter_Init();
    FlowMeter_Clear();

    // Initialize infrared human sensor
    Display_Clear();
    sprintf(Content, "Infrared Sensor Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    InfraredHuman_Init(InfraredHuman_Threshold);

    // Initialize infrared object sensor
    InfraredObject_Init(InfraredObject_Threshold);

    // Initialize NFC reader
    Display_Clear();
    sprintf(Content, "NFC Reader Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    NFC_Init();

    // Initialize temperature sensor
    Display_Clear();
    sprintf(Content, "Temperature Sensor Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    TemperatureSensor_Init();

    // Initialize valve control
    Display_Clear();
    sprintf(Content, "Valve Controller Initializing...");
    Display_ShowString(0, 0, Content, 84, FONTSIZE_8);
    Valve_Init();
    Valve_Stop();

    // Display initialization finished
    Display_Clear();
    sprintf(Content, "Initialized!");
    Display_ShowString(16, 1, Content, 84, FONTSIZE_16);
    Delay_s(5);
}

/**
 * @brief  Check acknowledge from destination.
 * @param  None.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerTerminal_ZigBeeCheck(void)
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

/**
 * @brief  Send information of terminal to host by ZigBee.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerTerminal_SendZigBeeData(uint8_t Timeout)
{
    uint8_t status = 0;
    uint8_t TransmitData[MAXZIGBEELENGTH];
    uint32_t StartTime = Information_GetTimeStamp();

    TransmitData[0] = '$';
    TransmitData[1] = ShowerTerminalSerial;
    Uint32toInt8(Terminal_Information.NFCSerial, &TransmitData[2], &TransmitData[3],
                 &TransmitData[4], &TransmitData[5]);
    Uint32toInt8(Terminal_Information.UserSerial, &TransmitData[6], &TransmitData[7],
                 &TransmitData[8], &TransmitData[9]);
    TransmitData[10] = Terminal_Information.TerminalState;
    Int32toInt8((int32_t)(Terminal_Information.WaterTemperature * 100.0), &TransmitData[11],
                &TransmitData[12], &TransmitData[13], &TransmitData[14]);
    Int32toInt8((int32_t)(Terminal_Information.WaterFlow * 100.0), &TransmitData[15],
                &TransmitData[16], &TransmitData[17], &TransmitData[18]);
    Int32toInt8((int32_t)(Terminal_Information.AccountBalance * 100.0), &TransmitData[19],
                &TransmitData[20], &TransmitData[21], &TransmitData[22]);
    TransmitData[23] = '$';

    do
    {
        Delay_ms(100);
        Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, TransmitData, 24);
        status = ShowerTerminal_ZigBeeCheck();
        if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
            return status;
    } while (status == 0); // Check ACK
    return status;
}

/**
 * @brief  Get data from host and resolve.
 * @param  Timeout: Set timeout in seconds.
 * @retval Status, 1 for success, 0 for failure.
 */
uint8_t ShowerTerminal_GetZigBeeData(uint8_t Timeout)
{
    uint8_t ReceivedData[MAXZIGBEELENGTH];
    uint16_t length = 0;
    uint8_t correct = 0;
    uint32_t StartTime = Information_GetTimeStamp();

    do
    {
        length = Communicate_ZigBeeRX(ReceivedData);
        if (length != 0)
        {
            if ((ReceivedData[0] == '$') && (ReceivedData[31] == '$')) // Whole frame
            {
                if ((ReceivedData[1] == ShowerTerminalSerial)) // Check address
                {
                    Terminal_Information.UserSerial = Int8toUint32(ReceivedData[2], ReceivedData[3], ReceivedData[4], ReceivedData[5]);
                    Terminal_Information.TerminalState = ReceivedData[6];
                    Terminal_Information.AccountBalance = (float)Int8toInt32(ReceivedData[7], ReceivedData[8], ReceivedData[9], ReceivedData[10]) / 100.0;

                    Terminal_Information.CurrentDate.RTC_Year = ReceivedData[11];
                    Terminal_Information.CurrentDate.RTC_Month = ReceivedData[12];
                    Terminal_Information.CurrentDate.RTC_Date = ReceivedData[13];
                    Terminal_Information.CurrentDate.RTC_WeekDay = ReceivedData[14];

                    Terminal_Information.CurrentTime.RTC_Hours = ReceivedData[15];
                    Terminal_Information.CurrentTime.RTC_Minutes = ReceivedData[16];
                    Terminal_Information.CurrentTime.RTC_Seconds = ReceivedData[17];
                    Terminal_Information.CurrentTime.RTC_H12 = ReceivedData[18];

                    Terminal_Information.ReserveStartTime.RTC_Hours = ReceivedData[19];
                    Terminal_Information.ReserveStartTime.RTC_Minutes = ReceivedData[20];
                    Terminal_Information.ReserveStartTime.RTC_Seconds = ReceivedData[21];
                    Terminal_Information.ReserveStartTime.RTC_H12 = ReceivedData[22];

                    Terminal_Information.ReserveStopTime.RTC_Hours = ReceivedData[23];
                    Terminal_Information.ReserveStopTime.RTC_Minutes = ReceivedData[24];
                    Terminal_Information.ReserveStopTime.RTC_Seconds = ReceivedData[25];
                    Terminal_Information.ReserveStopTime.RTC_H12 = ReceivedData[26];

                    Terminal_Information.ReservedUser = Int8toUint32(ReceivedData[27], ReceivedData[28], ReceivedData[29], ReceivedData[30]);

                    // ACK
                    sprintf(ACK, "$%cA$", ShowerTerminalSerial);
                    Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, ACK, 4);
                    ShowerTerminal_SetDevice();
                    correct = 1;
                }
            }
            else // Received but error
            {
                sprintf(NAK, "$%cN$", ShowerTerminalSerial);
                Communicate_ZigBeeTX(ZigBee_TypeCoordinator, FatherHostSerial, NAK, 4);
            }
        }
        if ((Information_GetTimeStamp() - StartTime) > Timeout) // Over time
            return correct;
    } while ((length == 0) || (correct != 1));

    return correct;
}

/**
 * @brief  Set terminal according to information.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_SetDevice(void)
{
    // Date & time
    Information_SetDateTime(&Terminal_Information.CurrentDate, &Terminal_Information.CurrentTime);
}

/**
 * @brief  Check whether there are objects left.
 * @param  None.
 * @retval Status, 1 for left, 0 for not.
 */
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

/**
 * @brief  Check whether need repair.
 * @param  None.
 * @retval Status, 1 for need, 0 for not.
 */
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

/**
 * @brief  Check whether need help.
 * @param  None.
 * @retval Status, 1 for need, 0 for not.
 */
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

/**
 * @brief  Check whether there is human.
 * @param  None.
 * @retval Status, 1 for human presence, 0 for not.
 */
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

/**
 * @brief  Check whether there is object.
 * @param  None.
 * @retval Status, 1 for object presence, 0 for not.
 */
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

/**
 * @brief  Check whether there is card.
 * @param  None.
 * @retval Status, 1 for card presence, 0 for not.
 */
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

/**
 * @brief  Check whether it is the first time swipe card.
 * @param  None.
 * @retval Status, 1 for fisrt read, 0 for not.
 */
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

/**
 * @brief  Check whether the card is taken.
 * @param  None.
 * @retval Status, 1 for card taken, 0 for not.
 */
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

/**
 * @brief  Check whether the user has reservation.
 * @param  None.
 * @retval Status, 1 for have, 0 for not.
 */
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

/**
 * @brief  Process the situation: objects are left.
 * @param  None.
 * @retval None.
 */
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

/**
 * @brief  Set help state to terminal state.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_SetHelpState(void)
{
    Terminal_Information.TerminalState = Terminal_Information.TerminalState | ShowerTerminal_Help;
}

/**
 * @brief  Set repair state to terminal state.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_SetRepairState(void)
{
    Terminal_Information.TerminalState = Terminal_Information.TerminalState | ShowerTerminal_Repair;
}

/**
 * @brief  Process the situation: first swipe card fail.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_CardFirstReadFailProcess(void)
{
    // Audio and display
    Audio_Play(Audio_Warn);
    Delay_ms(10);
    uint8_t Content[16] = "Query Fail!";
    ShowerTerminal_DisplayContent(Content);
    Delay_s(2);
    sprintf(Content, "Swipe Again!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(2);
}

/**
 * @brief  Process the situation: first swipe card.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_CardFirstReadProcess(void)
{
    // Audio and display
    Audio_Play(Audio_Remind);
    Delay_ms(10);
    uint8_t Content[] = "Wait...";
    ShowerTerminal_DisplayContent(Content);

    // Get card and user information
    Terminal_Information.NFCSerial = NFC_SetCheckCardNumber(0, NFC_CHECK);
    Terminal_Information.UserSerial = NFC_SetCheckUserNumber(0, NFC_CHECK);
    Terminal_Information.TerminalState |= ShowerTerminal_FirstReadCard; // Set flag

    // Query data
    uint8_t status;
    status = ShowerTerminal_SendZigBeeData(5); // 5s to ensure correct send
    if (status == 1)
    {
        status = ShowerTerminal_GetZigBeeData(5); // 5s to ensure correct receive
        if (status == 1)
        {
            Terminal_Information.TerminalState &= (~ShowerTerminal_FirstReadCard); // Clear flag
        }
        else
        {
            Terminal_Information.TerminalState &= (~ShowerTerminal_FirstReadCard); // Clear flag
            QueryFail = 1;
            ShowerTerminal_CardFirstReadFailProcess();
            return;
        }
    }
    else
    {
        Terminal_Information.TerminalState &= (~ShowerTerminal_FirstReadCard); // Clear flag
        QueryFail = 1;
        ShowerTerminal_CardFirstReadFailProcess();
        return;
    }

    // Preparation
    QueryFail = 0;
    if (ShowerTerminal_ReservationCheck())
    {
        // Valve on
        if (Valve_GetState() == 0)
        {
            Audio_Play(Audio_Remind);
            Delay_ms(10);
        }
        Valve_Start();

        // Flow meter on
        FlowMeter_Clear();
        FlowMeter_Start();
    }
}

/**
 * @brief  Process the situation: in use.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_InUseProcess(void)
{
    // Query fail
    if (QueryFail == 1)
    {
        return;
    }

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

/**
 * @brief  Process the situation: in use and water supply stopped.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_WaterStopRemind(void)
{
    uint8_t Content[17];
    if ((Terminal_Information.TerminalState & ShowerTerminal_WaterSupply) != ShowerTerminal_WaterSupply)
    {
        Audio_Play(Audio_NoWater);
        sprintf(Content, "Water Stop!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(5);
    }
}

/**
 * @brief  Process the situation: in use and near reservation time ends.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_NearReservationEnd(void)
{
    static uint16_t cnt = 0;
    uint8_t Content[17];
    uint32_t ReserveEndTime = Terminal_Information.ReserveStopTime.RTC_Hours * 3600 +
                              Terminal_Information.ReserveStopTime.RTC_Minutes * 60 +
                              Terminal_Information.ReserveStopTime.RTC_Seconds;
    uint32_t CurrentTime = Terminal_Information.CurrentTime.RTC_Hours * 3600 +
                           Terminal_Information.CurrentTime.RTC_Minutes * 60 +
                           Terminal_Information.CurrentTime.RTC_Seconds;
    if (((cnt % 15) == 0) && (CurrentTime >= ReserveEndTime))
    {
        Audio_Play(Audio_ReserveStopped);
        sprintf(Content, "Time Up!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(2);
    }
    if (((cnt % 45) == 0) && (ReserveEndTime - CurrentTime) <= 300)
    {
        Audio_Play(Audio_ReserveToStop);
        sprintf(Content, "Time Up Soon!");
        ShowerTerminal_DisplayContent(Content);
        Delay_s(2);
    }
    cnt++;
}

/**
 * @brief  Process the situation: no reservation.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_NoReservationProcess(void)
{
    uint8_t Content[17];
    Audio_Play(Audio_ReserveFirst);
    sprintf(Content, "No Appointment!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(2);
}

/**
 * @brief  Process the situation: card taken.
 * @param  None.
 * @retval None.
 */
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
        Audio_Play(Audio_Remind);
        Delay_ms(10);
    }
    Valve_Stop();

    // Display
    sprintf(Content, "Cost: %.2f", cost);
    ShowerTerminal_DisplayContent(Content);

    // Upload
    Terminal_Information.TerminalState |= ShowerTerminal_CardOff;
    ShowerTerminal_SendZigBeeData(0xFF); // 256s to ensure success
    ShowerTerminal_GetZigBeeData(2);
    Terminal_Information.TerminalState &= (~ShowerTerminal_CardOff);

    // Clear flag
    Terminal_Information.TerminalState &= (~ShowerTerminal_InUse);

    // Clear information
    Terminal_Information.NFCSerial = 0;
    Terminal_Information.UserSerial = 0;
    Terminal_Information.WaterTemperature = 0;
    Terminal_Information.WaterFlow = 0;
    Terminal_Information.AccountBalance = 0;

    Delay_s(1);
}

/**
 * @brief  Process the situation: human presence without swiping card.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_NotUseProcess(void)
{
    uint8_t Content[17];
    Audio_Play(Audio_SwipeCard);
    sprintf(Content, "Swipe Card!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(3);
}

/**
 * @brief  Process the situation: objects are left.
 * @param  None.
 * @retval None.
 */
void ShowerTerminal_CardObjectLeft(void)
{
    // Set flag
    Terminal_Information.TerminalState |= ShowerTerminal_ObjectLeft;

    uint8_t Content[17];
    Audio_Play(Audio_ObjectLeft);
    sprintf(Content, "Object Left!");
    ShowerTerminal_DisplayContent(Content);
    Delay_s(1);
}

/**
 * @brief  Process the situation: idle.
 * @param  None.
 * @retval None.
 */
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

/**
 * @brief  Display main contens with other information, like time, serial state...
 * @param  MainContent: Content to display.
 * @retval None.
 */
void ShowerTerminal_DisplayContent(uint8_t *MainContent)
{
    uint8_t Content[22];

    // Date and time
    Information_GetDateTime(&Terminal_Information.CurrentDate, &Terminal_Information.CurrentTime);
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
