/**
 ******************************************************************************
 * @file    Display.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.20
 * @brief   This file contains all the functions for display device.(For shower
 *          host.)
 * @note    Follow steps to use.
 *          - Use Display_Init() to initialize display device.
 *          - Use Display_On() and Display_Off() to turn on and turn off display.
 *          - Use Display_Clear() to clear screen, screen will be dark.
 *          - Use Display_LightUp() to light up all screen.
 *          - Use Display_SetPosition(uint8_t x, uint8_t y) to set display position.
 *          - Use Display_ShowChar(uint8_t x, uint8_t y, uint8_t data, uint8_t
 *              size) to show a char, position and size can be configured.
 *          - Use Display_ShowString(uint8_t x, uint8_t y, uint8_t *data,
 *              uint8_t size) to show a string, position and size can be configured.
 *          - Use Display_DrawBMP(uint8_t x1, uint8_t y1 ,uint8_t x2, uint8_t
 *              y2, uint8_t *data) to show an image of bmp within rectangular
 *              region determined by x1, y1, x2, y2.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Display.h"
#include "Display_Data.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Display data pin - PB7
#define DISPLAYDATA_CLOCK RCC_AHB1Periph_GPIOB
#define DISPLAYDATA_PINGROUP GPIOB
#define DISPLAYDATA_PIN GPIO_Pin_7

// Display clock pin - PB6
#define DISPLAYCLOCK_CLOCK RCC_AHB1Periph_GPIOB
#define DISPLAYCLOCK_PINGROUP GPIOB
#define DISPLAYCLOCK_PIN GPIO_Pin_6

// I2C function
#define I2CSDA_HIGH GPIO_SetBits(DISPLAYDATA_PINGROUP, DISPLAYDATA_PIN)
#define I2CSDA_LOW GPIO_ResetBits(DISPLAYDATA_PINGROUP, DISPLAYDATA_PIN)
#define I2CSCK_HIGH GPIO_SetBits(DISPLAYCLOCK_PINGROUP, DISPLAYCLOCK_PIN)
#define I2CSCK_LOW GPIO_ResetBits(DISPLAYCLOCK_PINGROUP, DISPLAYCLOCK_PIN)
#define I2CSDA_READ GPIO_ReadInputDataBit(DISPLAYDATA_PINGROUP, DISPLAYDATA_PIN)

// Control command
#define TRANSMIT_DATA 0x00
#define TRANSMIT_COMMAND 0x01
#define MAX_COLUMN 128
#define MAX_ROW 4 // 32 pixes in row, divided into 4 page, 1 byte for each page

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize GPIO pins and I2C1.
 * @param  None.
 * @retval None.
 */
void Display_HardwareInit(void)
{
    // GPIO initialize
    GPIO_InitTypeDef GPIO_InitStructure;
    // Display data pin - PB7
    RCC_AHB1PeriphClockCmd(DISPLAYDATA_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = DISPLAYDATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(DISPLAYDATA_PINGROUP, &GPIO_InitStructure);
    I2CSDA_HIGH;

    // Display clovk pin - PB6
    RCC_AHB1PeriphClockCmd(DISPLAYCLOCK_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = DISPLAYCLOCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(DISPLAYCLOCK_PINGROUP, &GPIO_InitStructure);
    I2CSCK_HIGH;
}

/**
 * @brief  Generate I2C start signal.
 * @param  None.
 * @retval None.
 */
void I2C_Start(void)
{
    // Ensure all high
    I2CSDA_HIGH;
    I2CSCK_HIGH;
    Delay_us(1);

    // Start signal
    I2CSDA_LOW;
    Delay_us(1);

    // Prepare for transmitting
    I2CSCK_LOW;
}

/**
 * @brief  Generate I2C stop signal.
 * @param  None.
 * @retval None.
 */
void I2C_Stop(void)
{
    // Ensure SDA low
    I2CSCK_LOW;
    I2CSDA_LOW;
    Delay_us(1);

    // Stop
    I2CSCK_HIGH;

    // Idle
    I2CSDA_HIGH;
    Delay_us(1);
}

/**
 * @brief  Check I2C ACK.
 * @param  None.
 * @retval 1 for NACK, 0 for ACK.
 */
uint8_t I2C_CheckACK(void)
{
    uint8_t WaitTime = 0;

    // Prepare to receive
    I2CSDA_HIGH;
    Delay_us(1);
    I2CSCK_HIGH;
    Delay_us(1);

    while (I2CSDA_READ)
    {
        WaitTime++;
        if (WaitTime > 250)
        {
            I2C_Stop();
            return 1;
        }
    }
    I2CSCK_LOW;
    return 0;
}

/**
 * @brief  Transmit 1 byte data.
 * @param  data: 1 byte data to transmit.
 * @retval None.
 */
void I2C_TransmitByte(uint8_t data)
{
    // Start bit
    I2CSCK_LOW;

    // Data
    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0x80) // Transmit "1"
        {
            I2CSDA_HIGH;
        }
        else // Transmit "0"
        {
            I2CSDA_LOW;
        }
        data = data << 1;
        Delay_us(1);
        I2CSCK_HIGH;
        Delay_us(1);
        I2CSCK_LOW;
        Delay_us(1);
    }
}

/**
 * @brief  Transmit data or command.
 * @param  content: content to transmit.
 * @param  type: TRANSMIT_COMMAND or TRANSMITT_DATA.
 * @retval 0 for success, 1 for not.
 */
uint8_t I2C_TransmitDataCommand(uint8_t content, uint8_t type)
{
    uint8_t type_choose;
    if (type == TRANSMIT_COMMAND)
    {
        type_choose = 0x00; // Command
    }
    else
    {
        type_choose = 0x40; // Data
    }

    // Start
    I2C_Start();

    // Address
    I2C_TransmitByte(0x78);
    if (I2C_CheckACK())
        return 1;

    // Choose type
    I2C_TransmitByte(type_choose);
    if (I2C_CheckACK())
        return 1;

    // Transmit content
    I2C_TransmitByte(content);
    if (I2C_CheckACK())
        return 1;

    // Stop
    I2C_Stop();
    return 0;
}

/**
 * @brief  Turn on display.
 * @param  None.
 * @retval None.
 */
void Display_On(void)
{
    I2C_TransmitDataCommand(0X8D, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(0X14, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(0XAF, TRANSMIT_COMMAND);
}

/**
 * @brief  Turn off display.
 * @param  None.
 * @retval None.
 */
void Display_Off(void)
{
    I2C_TransmitDataCommand(0X8D, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(0X10, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(0XAE, TRANSMIT_COMMAND);
}

/**
 * @brief  Clear screen. After using this function, screen will be all dark.
 * @param  None.
 * @retval None.
 */
void Display_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        I2C_TransmitDataCommand(0xB0 + i, TRANSMIT_COMMAND);
        I2C_TransmitDataCommand(0x00, TRANSMIT_COMMAND);
        I2C_TransmitDataCommand(0x10, TRANSMIT_COMMAND);
        for (n = 0; n < 128; n++)
            I2C_TransmitDataCommand(0, TRANSMIT_DATA);
    }
}

/**
 * @brief  Light up screen. After using this function, screen will be all dark.
 * @param  None.
 * @retval None.
 */
void Display_LightUp(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        I2C_TransmitDataCommand(0xB0 + i, TRANSMIT_COMMAND);
        I2C_TransmitDataCommand(0x00, TRANSMIT_COMMAND);
        I2C_TransmitDataCommand(0x10, TRANSMIT_COMMAND);
        for (n = 0; n < 128; n++)
            I2C_TransmitDataCommand(0xFF, TRANSMIT_DATA);
    }
}

/**
 * @brief  Set display position.
 * @param  x: Position x.
 * @param  y: Position y.
 * @retval None.
 */
void Display_SetPosition(uint8_t x, uint8_t y)
{
    I2C_TransmitDataCommand(0xB0 + y, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(((x & 0xF0) >> 4) | 0x10, TRANSMIT_COMMAND);
    I2C_TransmitDataCommand(x & 0x0F, TRANSMIT_COMMAND);
}

/**
 * @brief  Show char on screen.
 * @param  x: Position x.
 * @param  y: Position y.
 * @param  data: Data to display.
 * @param  size: Size of char, FONTSIZE_16 or FONTSIZE_6
 * @retval None.
 */
void Display_ShowChar(uint8_t x, uint8_t y, uint8_t data, uint8_t size)
{
    uint8_t ch = 0, i = 0;
    ch = data - ' '; // Calculate the position in array

    if (size == FONTSIZE_16)
    {
        if (ch >= 95) // Non-printing characters, display space
        {
            ch = 0;
        }
        if (x > MAX_COLUMN - 8)
        {
            x = 0;
        }
        if (y > MAX_ROW - 2)
        {
            y = 0;
        }
        Display_SetPosition(x, y);
        for (i = 0; i < 8; i++)
            I2C_TransmitDataCommand(F8X16[ch][i], TRANSMIT_DATA);
        Display_SetPosition(x, y + 1);
        for (i = 8; i < 16; i++)
            I2C_TransmitDataCommand(F8X16[ch][i], TRANSMIT_DATA);
    }
    else
    {
        if (ch >= 92) // Non-printing characters, display space
        {
            ch = 0;
        }
        if (x > MAX_COLUMN - 6)
        {
            x = 0;
        }
        if (y > MAX_ROW - 1)
        {
            y = 0;
        }
        Display_SetPosition(x, y);
        for (i = 0; i < 6; i++)
            I2C_TransmitDataCommand(F6x8[ch][i], TRANSMIT_DATA);
    }
}

/**
 * @brief  Show string on screen.
 * @param  x: Position x.
 * @param  y: Position y.
 * @param  data: Data to display.
 * @param  size: Size of char, FONTSIZE_16 or FONTSIZE_6
 * @retval None.
 */
void Display_ShowString(uint8_t x, uint8_t y, uint8_t *data, uint8_t size)
{
    uint8_t i = 0;
    if (size == FONTSIZE_16)
    {
        while (data[i] != '\0')
        {
            Display_ShowChar(x, y, data[i], size);
            x += 8;
            if (x > MAX_COLUMN - 8) // New line
            {
                x = 0;
                y += 2;
            }
            if (y > MAX_ROW - 2) // Over display region
                return;
            i++;
        }
    }
    else
    {
        while (data[i] != '\0')
        {
            Display_ShowChar(x, y, data[i], size);
            x += 6;
            if (x > MAX_COLUMN - 6) // New line
            {
                x = 0;
                y++;
            }
            if (y > MAX_ROW - 1) // Over display region
                return;
            i++;
        }
    }
}

/**
 * @brief  Draw a bmp image on screen.
 * @param  x1: Left position of image.
 * @param  y1: Top position of image.
 * @param  x2: Right position of image.
 * @param  y2: Top position of image
 * @param  data: Data to display.
 * @retval None.
 */
void Display_DrawBMP(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *data)
{
    uint8_t tempX = 0, tempY = 0;
    uint16_t num = 0;
    for (tempY = y1; (tempY <= y2) && (tempY < MAX_ROW); tempY++)
    {
        Display_SetPosition(x1, tempY);
        for (tempX = x1; (tempX <= x2) && (tempX < MAX_COLUMN); tempX++)
        {
            I2C_TransmitDataCommand(data[num++], TRANSMIT_DATA);
        }
    }
}

/**
 * @brief  Initialize display device.
 * @param  None.
 * @retval None.
 */
void Display_Init(void)
{
    Display_HardwareInit();
    Delay_ms(1);

    // Initialize command
    Delay_ms(200);
    I2C_TransmitDataCommand(0xAE, TRANSMIT_COMMAND); // Turn off display

    I2C_TransmitDataCommand(0x00, TRANSMIT_COMMAND); // Set lower column address
    I2C_TransmitDataCommand(0x10, TRANSMIT_COMMAND); // Set higher column address

    I2C_TransmitDataCommand(0x40, TRANSMIT_COMMAND); // Set display start line

    I2C_TransmitDataCommand(0xB0, TRANSMIT_COMMAND); // Set page address

    I2C_TransmitDataCommand(0x81, TRANSMIT_COMMAND); // Contract control
    I2C_TransmitDataCommand(0xFF, TRANSMIT_COMMAND); // Brightness 128

    I2C_TransmitDataCommand(0xA0, TRANSMIT_COMMAND); // Set segment remap

    I2C_TransmitDataCommand(0xA6, TRANSMIT_COMMAND); // Normal mode, 1 for bright, 0 for dark

    I2C_TransmitDataCommand(0xA8, TRANSMIT_COMMAND); // Multiplex ratio
    I2C_TransmitDataCommand(0x1F, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xC0, TRANSMIT_COMMAND); // Com scan direction

    I2C_TransmitDataCommand(0xD3, TRANSMIT_COMMAND); // Set display offset
    I2C_TransmitDataCommand(0x00, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xD5, TRANSMIT_COMMAND); // Set osc division
    I2C_TransmitDataCommand(0x00, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xD9, TRANSMIT_COMMAND); // Set pre-charge period
    I2C_TransmitDataCommand(0x1F, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xDA, TRANSMIT_COMMAND); // Set COM pins
    I2C_TransmitDataCommand(0x02, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xDB, TRANSMIT_COMMAND); // Set Vcomh
    I2C_TransmitDataCommand(0x30, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0x8D, TRANSMIT_COMMAND); // Set charge pump enable
    I2C_TransmitDataCommand(0x14, TRANSMIT_COMMAND);

    I2C_TransmitDataCommand(0xAF, TRANSMIT_COMMAND);
    Display_Clear();
}
/***********************************END OF FILE********************************/
