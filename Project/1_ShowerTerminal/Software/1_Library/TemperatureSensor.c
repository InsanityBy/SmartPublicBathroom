/**
 ******************************************************************************
 * @file    TemperatureSensor.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.7
 * @brief   This file contains all the functions of the temperature sensor.(For
 *         shower terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "TemperatureSensor.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SENSOR_CLOCK RCC_AHB1Periph_GPIOA
#define SENSOR_PINGROUP GPIOA
#define SENSOR_PIN GPIO_Pin_1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Change pin mode to input.
 * @param  None.
 * @retval None.
 */
void Pin_Input()
{
    RCC_AHB1PeriphClockCmd(SENSOR_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SENSOR_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Change pin mode to output.
 * @param  None.
 * @retval None.
 */
void Pin_Output()
{
    RCC_AHB1PeriphClockCmd(SENSOR_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SENSOR_PINGROUP, &GPIO_InitStructure);
}

/**
 * @brief  Reset DS18B20 temperature sensor.
 * @param  None.
 * @retval None.
 */
void Sensor_Reset(void)
{
    // Send reset pulse
    Pin_Output();
    GPIO_ResetBits(SENSOR_PINGROUP, SENSOR_PIN);
    Delay_us(1000);
    GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN);
    // Wait sensor to respond
    Delay_us(15);
}

/**
 * @brief  Check DS18B20 temperature sensor presence.
 * @param  None.
 * @retval The state of sensor, 1 for not exist, 0 for exist.
 */
uint8_t Sensor_Check(void)
{
    // Check 60~240us pulse sent by sensor, maybe wait another 45us or so before pulse
    uint8_t CheckNum = 0;
    Pin_Input();
    while (GPIO_ReadInputDataBit(SENSOR_PINGROUP, SENSOR_PIN) && (CheckNum < 50))
    {
        CheckNum++;
        Delay_us(1);
    }
    if (CheckNum >= 50)
        return 1;
    else
    {
        Delay_us(500 - CheckNum); // At least 480us
        return 0;
    }
}

/**
 * @brief  Read 1 bit from DS18B20 temperature sensor.
 * @param  None.
 * @retval uint8_t but only 1 bit data from sensor.
 */
uint8_t Sensor_ReadBit(void)
{
    uint8_t data;
    // Start transmit
    Pin_Output();
    GPIO_ResetBits(SENSOR_PINGROUP, SENSOR_PIN);
    Delay_us(2);
    GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN);

    // Sample
    Pin_Input();
    Delay_us(8);
    if (GPIO_ReadInputDataBit(SENSOR_PINGROUP, SENSOR_PIN))
        data = 1;
    else
        data = 0;
    Delay_us(60);
    return data;
}

/**
 * @brief  Read 1 byte from DS18B20 temperature sensor.
 * @param  None.
 * @retval 1 byte data from sensor.
 */
uint8_t Sensor_ReadByte(void)
{
    uint8_t data, temp;
    data = 0;
    for (int i = 0; i < 8; i++)
    {
        temp = Sensor_ReadBit();
        data = (temp << 7) | (data >> 1); // LSB transmit
    }
    return data;
}

/**
 * @brief  Write 1 bit to DS18B20 temperature sensor.
 * @param  data: 1 bit data to send.
 * @retval None.
 */
void Sensor_WriteBit(uint8_t data)
{
    Pin_Output();
    if (data)
    {
        GPIO_ResetBits(SENSOR_PINGROUP, SENSOR_PIN); // Write 1
        Delay_us(5);
        GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN);
        Delay_us(60);
    }
    else
    {
        GPIO_ResetBits(SENSOR_PINGROUP, SENSOR_PIN); // Write 0
        Delay_us(60);
        GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN);
        Delay_us(5);
    }
}

/**
 * @brief  Write 1 byte to DS18B20 temperature sensor.
 * @param  data: 1 byte data to send.
 * @retval None.
 */
void Sensor_WriteByte(uint8_t data)
{
    uint8_t temp;
    for (int i = 0; i < 8; i++)
    {
        temp = data & 0x01;
        data = data >> 1;
        Sensor_WriteBit(temp);
    }
}

/**
 * @brief  Start temperature convert.
 * @param  None.
 * @retval None.
 */
void Sensor_Convert(void)
{
    Sensor_Reset();
    Sensor_Check();
    Sensor_WriteByte(0xCC); // Skip ROM
    Sensor_WriteByte(0x44); // Start convert
}

/**
 * @brief  Get temperature.
 * @param  None.
 * @retval Temperature from sensor.
 */
float Sensor_GetTemperature(void)
{
    uint8_t DataLow, DataHigh;
    uint16_t Data;
    float Temperature;

    Sensor_Reset();
    Sensor_Check();
    Sensor_WriteByte(0xCC); // Skip ROM
    Sensor_WriteByte(0xBE); // Read scrachpad
    DataLow = Sensor_ReadByte();
    DataHigh = Sensor_ReadByte();

    if (DataHigh > 7)
    {
        DataLow = ~DataLow;
        DataHigh = ~DataHigh;
        Data = DataHigh;
        Data = Data << 8;
        Data += DataLow;
        Temperature = (float)Data * -0.0625;
    }
    else
    {
        Data = DataHigh;
        Data = Data << 8;
        Data += DataLow;
        Temperature = (float)Data * 0.0625;
    }
    return Temperature;
}

/**
 * @brief  Initialize temperature sensor, check sensor presence.
 * @param  None.
 * @retval The state of sensor, 1 for not exist, 0 for exist.
 */
uint8_t TemperatureSensor_Init(void)
{
    // GPIO initialize
    RCC_AHB1PeriphClockCmd(SENSOR_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SENSOR_PINGROUP, &GPIO_InitStructure);

    // Idle state
    GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN); // 1-wire idle state is high

    // Sensor reset
    Sensor_Reset();

    // Sensor presence check
    return Sensor_Check();
}

/***********************************END OF FILE********************************/
