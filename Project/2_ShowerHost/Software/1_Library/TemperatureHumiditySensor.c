/**
 ******************************************************************************
 * @file    TemperatureHumiditySensor.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.9
 * @brief   This file contains all the functions for temperature - humidity
 *         sensor.(For shower host.)
 * @note    Follow steps to use.
 *          - Use TemperatureHumiditySensor_Init() to initialize device.
 *          - Use Sensor_Convert() to start convert.
 *          - Use Sensor_GetTemperature() to get current temperature. MUST
 *              START CONVERT BEFORE GET TEMPERATURE.
 *          - Use Sensor_GetHumidity() to get current humidity. MUST START
 *              CONVERT BEFORE GET TEMPERATURE.
 *          - USe Sensor_Getstate() to get current state of tempertature and
 *              humidity, 1 for fan should be turned on, 0 for not.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "TemperatureHumiditySensor.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Temperature humidity sensor input pin - PA1
#define SENSOR_CLOCK RCC_AHB1Periph_GPIOA
#define SENSOR_PINGROUP GPIOA
#define SENSOR_PIN GPIO_Pin_1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
float Temperature;
float Humidity;
float Temperature_Threshold = 26;
float Humidity_Threshold = 30;
uint16_t OwHumA, OwHumB;

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
 * @brief  Reset DHT21 temperature - humidity sensor.
 * @param  None.
 * @retval None.
 */
void Sensor_Reset(void)
{
    // Send reset pulse
    Pin_Output();
    GPIO_ResetBits(SENSOR_PINGROUP, SENSOR_PIN);
    Delay_us(960);
    GPIO_SetBits(SENSOR_PINGROUP, SENSOR_PIN);
    // Wait sensor to respond
    Delay_us(15);
}

/**
 * @brief  Check DHT21 temperature - humidity sensor presence.
 * @param  None.
 * @retval The state of sensor, 1 for not exist, 0 for exist.
 */
uint8_t Sensor_Check(void)
{
    // Check 60~240us pulse sent by sensor, maybe wait another 45us or so before pulse
    uint8_t CheckNum = 0;
    Pin_Input();
    while (GPIO_ReadInputDataBit(SENSOR_PINGROUP, SENSOR_PIN) && (CheckNum <= 45))
    {
        CheckNum++;
        Delay_us(1);
    }
    if (CheckNum > 45)
        return 1;
    else
    {
        Delay_us(290 - CheckNum); // Wait 45us(max), pulse 240us(max), recover 5us(typ)
        return 0;
    }
}

/**
 * @brief  Read 1 bit from DHT21 temperature - humidity sensor.
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
    Delay_us(50);
    return data;
}

/**
 * @brief  Read 1 byte from DHT21 temperature - humidity sensor.
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
 * @brief  Write 1 bit to DHT21 temperature - humidity sensor.
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
 * @brief  Write 1 byte to DHT21 temperature - humidity sensor.
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
 * @brief  Start DHT21 temperature - humidity convert.
 * @param  None.
 * @retval None.
 */
void Sensor_Convert(void)
{
    // Get calibration value
    Sensor_Reset();
    Sensor_Check();
    Sensor_WriteByte(0xCC);
    Sensor_WriteByte(0xDD);
    OwHumA = Sensor_ReadByte();
    OwHumA = (OwHumA << 8) | Sensor_ReadByte();
    OwHumB = Sensor_ReadByte();
    OwHumB = (OwHumB << 8) | Sensor_ReadByte();

    // Start convert
    Sensor_Reset();
    Sensor_Check();
    Sensor_WriteByte(0xCC);
    Sensor_WriteByte(0x10);
}

/**
 * @brief  Get temperature.
 * @param  None.
 * @retval Temperature from sensor.
 */
float Sensor_GetTemperature(void)
{
    uint8_t DataL, DataH;
    int16_t Data;

    Sensor_Reset();
    Sensor_Check();
    Sensor_WriteByte(0xCC);
    Sensor_WriteByte(0xBD); // Get temperature
    DataL = Sensor_ReadByte();
    DataH = Sensor_ReadByte();
    Data = DataH;
    Data = (Data << 8) | DataL;
    Data = 400 + 10 * Data / 256;
    Temperature = (float)Data / 10;

    return Temperature;
}

/**
 * @brief  Get humidity.
 * @param  None.
 * @retval Humidity from sensor.
 */
float Sensor_GetHumidity(void)
{
    uint8_t DataL, DataH;
    int16_t DataS16;
    int32_t DataS32;

    Sensor_Reset();
    Sensor_Check();

    // Get temperature first
    Sensor_WriteByte(0xCC);
    Sensor_WriteByte(0xBD); // Get temperature
    DataL = Sensor_ReadByte();
    DataH = Sensor_ReadByte();
    DataS16 = DataH;
    DataS16 = (DataS16 << 8) | DataL;
    DataS16 = 400 + 10 * DataS16 / 256;

    // Get humidity
    DataL = Sensor_ReadByte();
    DataH = Sensor_ReadByte();
    DataS32 = DataH;
    DataS32 = (DataS32 << 8) | DataL;
    DataS32 = (DataS32 - OwHumB) * 600 / (OwHumA - OwHumB) + 300;
    DataS32 = DataS32 + 25 * (DataS16 - 250) / 100;
    Humidity = (float)DataS32 / 10;
    if (Humidity > 99.9)
        Humidity = 99.9;
    else if (Humidity < 0)
        Humidity = 0;

    return Humidity;
}

/**
 * @brief  Get temperature and humidity state.
 * @param  None.
 * @retval 1 for fan should be on and 0 for off.
 */
uint8_t Sensor_Getstate(void)
{
    float temperature_current, humidity_current;
    Sensor_Convert();
    temperature_current = Sensor_GetTemperature();
    Sensor_Convert();
    humidity_current = Sensor_GetHumidity();

    if ((temperature_current > Temperature_Threshold) || (humidity_current > Humidity_Threshold))
    {
        return 1;
    }
    return 0;
}

/**
 * @brief  Initialize temperature sensor, check sensor presence.
 * @param  None.
 * @retval The state of sensor, 1 for not exist, 0 for exist.
 */
uint8_t TemperatureHumiditySensor_Init(void)
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
