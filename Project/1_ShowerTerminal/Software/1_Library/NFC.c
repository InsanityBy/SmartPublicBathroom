/**
 ******************************************************************************
 * @file    NFC.c
 * @author  Ma boyang
 * @version V1.0
 * @date    2022.5.16
 * @brief   This file contains all the functions for NFC reader.(For shower
 *          terminal.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "NFC.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// RST pin - PA7
#define NFC_RST_CLOCK RCC_AHB1Periph_GPIOA
#define NFC_RST_PINGROUP GPIOA
#define NFC_RST_PIN GPIO_Pin_7

// NSS pin - PB12
#define NFC_NSS_CLOCK RCC_AHB1Periph_GPIOB
#define NFC_NSS_PINGROUP GPIOB
#define NFC_NSS_PIN GPIO_Pin_12

// SCK pin - PB13
#define NFC_SCK_CLOCK RCC_AHB1Periph_GPIOB
#define NFC_SCK_PINGROUP GPIOB
#define NFC_SCK_PIN GPIO_Pin_13
#define NFC_SCK_PINSOURCE GPIO_PinSource13

// MISO pin - PB14
#define NFC_MISO_CLOCK RCC_AHB1Periph_GPIOB
#define NFC_MISO_PINGROUP GPIOB
#define NFC_MISO_PIN GPIO_Pin_14
#define NFC_MISO_PINSOURCE GPIO_PinSource14

// MOSI pin - PB15
#define NFC_MOSI_CLOCK RCC_AHB1Periph_GPIOB
#define NFC_MOSI_PINGROUP GPIOB
#define NFC_MOSI_PIN GPIO_Pin_15
#define NFC_MOSI_PINSOURCE GPIO_PinSource15

// IRQ pin - PB0
#define NFC_IRQ_CLOCK RCC_AHB1Periph_GPIOB
#define NFC_IRQ_PINGROUP GPIOB
#define NFC_IRQ_PIN GPIO_Pin_0

// MF522 command
#define PCD_IDLE 0x00        // Cancle command, idle state
#define PCD_AUTHENT 0x0E     // Verify
#define PCD_RECEIVE 0x08     // Receive data
#define PCD_TRANSMIT 0x04    // Transmit data
#define PCD_TRANSCEIVE 0x0C  // Receive and transmit data
#define PCD_RESETPHASE 0x0F  // Reset
#define PCD_CALCCRC 0x03     // Calculate CRC
#define PCD_NOCMDCHANGE 0x07 // No command change

// Mifare_One card command
#define PICC_REQIDL 0x26    // Find cards not sleep within range
#define PICC_REQALL 0x52    // Find all cards within range
#define PICC_ANTICOLL1 0x93 // Anti-collision
#define PICC_ANTICOLL2 0x95 // Anti-collision
#define PICC_AUTHENT1A 0x60 // Verify key A
#define PICC_AUTHENT1B 0x61 // Verify key B
#define PICC_READ 0x30      // Read block
#define PICC_WRITE 0xA0     // Write block
#define PICC_DECREMENT 0xC0 // Decrement data
#define PICC_INCREMENT 0xC1 // Increment data
#define PICC_RESTORE 0xC2   // Save data to buffer
#define PICC_TRANSFER 0xB0  // Transfer data from buffer to block
#define PICC_HALT 0x50      // Halt

// MF522 FIFO length
#define DEF_FIFO_LENGTH 64 // FIFO size = 64byte

// MF522 registor
// PAGE 0
#define RFU00 0x00
#define CommandReg 0x01
#define ComIEnReg 0x02
#define DivlEnReg 0x03
#define ComIrqReg 0x04
#define DivIrqReg 0x05
#define ErrorReg 0x06
#define Status1Reg 0x07
#define Status2Reg 0x08
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0A
#define WaterLevelReg 0x0B
#define ControlReg 0x0C
#define BitFramingReg 0x0D
#define CollReg 0x0E
#define RFU0F 0x0F
// PAGE 1
#define RFU10 0x10
#define ModeReg 0x11
#define TxModeReg 0x12
#define RxModeReg 0x13
#define TxControlReg 0x14
#define TxAutoReg 0x15
#define TxSelReg 0x16
#define RxSelReg 0x17
#define RxThresholdReg 0x18
#define DemodReg 0x19
#define RFU1A 0x1A
#define RFU1B 0x1B
#define MifareReg 0x1C
#define RFU1D 0x1D
#define RFU1E 0x1E
#define SerialSpeedReg 0x1F
// PAGE 2
#define RFU20 0x20
#define CRCResultRegM 0x21
#define CRCResultRegL 0x22
#define RFU23 0x23
#define ModWidthReg 0x24
#define RFU25 0x25
#define RFCfgReg 0x26
#define GsNReg 0x27
#define CWGsCfgReg 0x28
#define ModGsCfgReg 0x29
#define TModeReg 0x2A
#define TPrescalerReg 0x2B
#define TReloadRegH 0x2C
#define TReloadRegL 0x2D
#define TCounterValueRegH 0x2E
#define TCounterValueRegL 0x2F
// PAGE 3
#define RFU30 0x30
#define TestSel1Reg 0x31
#define TestSel2Reg 0x32
#define TestPinEnReg 0x33
#define TestPinValueReg 0x34
#define TestBusReg 0x35
#define AutoTestReg 0x36
#define VersionReg 0x37
#define AnalogTestReg 0x38
#define TestDAC1Reg 0x39
#define TestDAC2Reg 0x3A
#define TestADCReg 0x3B
#define RFU3C 0x3C
#define RFU3D 0x3D
#define RFU3E 0x3E
#define RFU3F 0x3F

#define REQ_ALL 0x52
#define KEYA 0x60

// Response
#define MFRC_OK (char)0
#define MFRC_NOTAGERR (char)(-1)
#define MFRC_ERR (char)(-2)

#define MAXRLEN 18

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*===============================================================================
***********************Register read and write functions***********************
================================================================================*/
/**
 * @brief  Write data and read recent data using SPI2.
 * @param  data: Data to be wrote.
 * @retval Recent data from slave.
 */
uint8_t SPI2_WriteReadByte(uint8_t data)
{

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;                         // Transmit buffer empty
    SPI_I2S_SendData(SPI2, data); // Transmit data
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
        ;                             // Receive buffer not empty
    return SPI_I2S_ReceiveData(SPI2); // Recent data from slave
}

/**
 * @brief  Read data from a registor.
 * @param  address: Address of the registor to be read
 * @retval Data from the registor.
 */
uint8_t MFRC_ReadReg(uint8_t address)
{
    uint8_t address_real, data;
    address_real = ((address << 1) & 0x7E) | 0x80; // Address byte
    GPIO_ResetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN); // Enable slave
    SPI2_WriteReadByte(address_real);              // Write address
    data = SPI2_WriteReadByte(0xFF);               // Read data, write any data
    GPIO_SetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN);   // Disable slave
    return data;
}

/**
 * @brief  Write data to a registor.
 * @param  address: Address of the registor to be wrote
 * @param  data: Data to be wrote.
 * @retval None.
 */
void MFRC_WriteReg(uint8_t address, uint8_t data)
{
    uint8_t address_real;
    address_real = (address << 1) & 0x7E;          // Address byte
    GPIO_ResetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN); // Enable slave
    SPI2_WriteReadByte(address_real);              // Write address
    SPI2_WriteReadByte(data);                      // Write data
    GPIO_SetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN);   // Disable slave
}

/**
 * @brief  Set bit in a registor, different bits can be set the same time.
 * @param  address: Address of the registor to be oprate.
 * @param  mask: Bits to be set.
 * @retval None.
 */
void MFRC_SetBits(uint8_t address, uint8_t mask)
{
    uint8_t temp;
    temp = MFRC_ReadReg(address);        // Read data from registor
    MFRC_WriteReg(address, temp | mask); // Write set data to regitor
}

/**
 * @brief  Reset bit in a registor, different bits can be reset the same time.
 * @param  address: Address of the registor to be oprate.
 * @param  mask: Bits to be reset.
 * @retval None.
 */
void MFRC_ResetBits(uint8_t address, uint8_t mask)
{
    uint8_t temp;
    temp = MFRC_ReadReg(address);           // Read data from registor
    MFRC_WriteReg(address, temp & (~mask)); // Write reset data to regitor
}

/*===============================================================================
*************************RC522 basic operation functions*************************
================================================================================*/
/**
 * @brief  Calculate CRC.
 * @param  pInData: Pointer of the data to be wrote.
 * @param  InLenByte: Length of the data to be wrote.
 * @param  pOutData: Pointer of the data to be read.
 * @retval None.
 */
void CalculateCRC(uint8_t *pInData, uint8_t InLenByte, uint8_t *pOutData)
{
    uint8_t i, n;
    MFRC_ResetBits(DivIrqReg, 0x04);
    MFRC_WriteReg(CommandReg, PCD_IDLE);
    MFRC_SetBits(FIFOLevelReg, 0x80);
    for (i = 0; i < InLenByte; i++)
    {
        MFRC_WriteReg(FIFODataReg, *(pInData + i));
    }
    MFRC_WriteReg(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = MFRC_ReadReg(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));
    pOutData[0] = MFRC_ReadReg(CRCResultRegL);
    pOutData[1] = MFRC_ReadReg(CRCResultRegM);
}

/**
 * @brief  Turn on antenna.
 * @param  None.
 * @retval None.
 * @note   Time between turn on and off antenna should be no less than 1ms
 */
void PCD_AntennaOn(void)
{
    uint8_t temp;
    temp = MFRC_ReadReg(TxControlReg);
    if (!(temp & 0x03))
    {
        MFRC_SetBits(TxControlReg, 0x03);
    }
}

/**
 * @brief  Turn off antenna.
 * @param  None.
 * @retval None.
 * @note   Time between turn on and off antenna should be no less than 1ms
 */
void PCD_AntennaOff(void)
{
    MFRC_ResetBits(TxControlReg, 0x03);
}

/**
 * @brief  Communicate with card by RC522.
 * @param  cmd: Command to be wrote to card.
 * @param  pInData: Pointer of the data to be wrote.
 * @param  InLenByte: Length of the data to be wrote.
 * @param  pOutData: Pointer of the data to be read.
 * @param  pOutLenBit: Pointer of length of the data bit.
 * @retval Status: OK, NOTAGERR, ERR.
 */
char MFRC_CmdFrame(uint8_t cmd, uint8_t *pInData, uint8_t InLenByte, uint8_t *pOutData, uint16_t *pOutLenBit)
{
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
    char status = MFRC_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitFor = 0x00;

    // Set flag bit according to command
    switch (cmd)
    {
    case PCD_AUTHENT: // Verify
        irqEn = 0x12;
        waitFor = 0x10; // idleIRQ interrupt flag
        break;
    case PCD_TRANSCEIVE: // Transmit and receive data
        irqEn = 0x77;
        waitFor = 0x30; // RxIRQ and idleIRQ interrupt flag
        break;
    default:
        break;
    }

    // Prepare for transmitting command
    MFRC_WriteReg(ComIEnReg, irqEn | 0x80); // Enable interrupt
    MFRC_ResetBits(ComIrqReg, 0x80);        // Clear interrupt flag SET1
    MFRC_WriteReg(CommandReg, PCD_IDLE);    // Go into idle
    MFRC_SetBits(FIFOLevelReg, 0x80);       // Clear FIFO and its flag

    // Transmit command
    for (i = 0; i < InLenByte; i++) // Write command parameter
    {
        MFRC_WriteReg(FIFODataReg, pInData[i]);
    }
    MFRC_WriteReg(CommandReg, cmd); // Excute command
    if (cmd == PCD_TRANSCEIVE)
    {
        MFRC_SetBits(BitFramingReg, 0x80); // Transmit
    }
    i = 1000; // According to clock frequency, oprate M1 card no more than 25ms
    do
    {
        n = MFRC_ReadReg(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor)); // Wait
    MFRC_ResetBits(BitFramingReg, 0x80);                 // Stop

    // Process received data
    if (i != 0)
    {
        if (!(MFRC_ReadReg(ErrorReg) & 0x1B))
        {
            status = MFRC_OK;
            if (n & irqEn & 0x01)
            {
                status = MFRC_NOTAGERR;
            }
            if (cmd == PCD_TRANSCEIVE)
            {
                n = MFRC_ReadReg(FIFOLevelReg);
                lastBits = MFRC_ReadReg(ControlReg) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = MFRC_ReadReg(FIFODataReg);
                }
            }
        }
        else
        {
            status = MFRC_ERR;
        }
    }

    MFRC_SetBits(ControlReg, 0x80);      // Stop timer
    MFRC_WriteReg(CommandReg, PCD_IDLE); // Go into idle

    return status;
}

/**
 * @brief  Config mode.
 * @param  type: mode.
 * @retval Status: OK for success.
 */
char M500PcdConfigISOType(uint8_t type)
{
    if (type == 'A') // ISO14443_A
    {
        MFRC_ResetBits(Status2Reg, 0x08);
        MFRC_WriteReg(ModeReg, 0x3D); // 3F

        MFRC_WriteReg(RxSelReg, 0x86);  // 84
        MFRC_WriteReg(RFCfgReg, 0x7F);  // 4F
        MFRC_WriteReg(TReloadRegL, 30); // tmoLength
        MFRC_WriteReg(TReloadRegH, 0);
        MFRC_WriteReg(TModeReg, 0x8D);
        MFRC_WriteReg(TPrescalerReg, 0x3E);

        Delay_us(10);
        PCD_AntennaOn();
    }
    else
    {
        return (char)-1;
    }

    return MFRC_OK;
}

/*===============================================================================
*************************RC522 basic operation functions*************************
================================================================================*/
/**
 * @brief  Find card within reach.
 * @param  RequestMode: Command to be wrote to card.
 *                      PICC_REQIDL: Find cards not sleep.
 *                      PICC_REQALL: Find all cards.
 * @param  pCardType: Pointer of the card type.
 *                  0x4400 = Mifare_UltraLight
 *                  0x0400 = Mifare_One(S50)
 *                  0x0200 = Mifare_One(S70)
 *                  0x0800 = Mifare_Pro(X)
 *                  0x4403 = Mifare_DESFire
 * @retval Status: OK for success.
 */
char PCD_Request(uint8_t RequestMode, uint8_t *pCardType)
{
    int status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MAXRLEN];

    MFRC_ResetBits(Status2Reg, 0x08);
    MFRC_WriteReg(BitFramingReg, 0x07);
    MFRC_SetBits(TxControlReg, 0x03);

    CmdFrameBuf[0] = RequestMode;

    status = MFRC_CmdFrame(PCD_TRANSCEIVE, CmdFrameBuf, 1, CmdFrameBuf, &unLen);

    if ((status == MFRC_OK) && (unLen == 0x10))
    {
        *pCardType = CmdFrameBuf[0];
        *(pCardType + 1) = CmdFrameBuf[1];
    }
    else
    {
        status = MFRC_ERR;
    }

    return status;
}

/**
 * @brief  Prevent cards from collision.
 * @param  pSnr: Card serial number.
 * @retval Status: OK for success.
 */
char PCD_Anticoll(uint8_t *pSnr)
{
    char status;
    uint8_t i, snr_check = 0;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MAXRLEN];

    MFRC_ResetBits(Status2Reg, 0x08);
    MFRC_WriteReg(BitFramingReg, 0x00);
    MFRC_ResetBits(CollReg, 0x80);

    CmdFrameBuf[0] = PICC_ANTICOLL1;
    CmdFrameBuf[1] = 0x20;

    status = MFRC_CmdFrame(PCD_TRANSCEIVE, CmdFrameBuf, 2, CmdFrameBuf, &unLen);
    if (status == MFRC_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(pSnr + i) = CmdFrameBuf[i];
            snr_check ^= CmdFrameBuf[i];
        }
        if (snr_check != CmdFrameBuf[i])
        {
            status = MFRC_ERR;
        }
    }

    MFRC_SetBits(CollReg, 0x80);
    return status;
}

/**
 * @brief  Select cards.
 * @param  pSnr: Card serial number.
 * @retval Status: OK for success.
 */
char PCD_Select(uint8_t *pSnr)
{
    char status;
    uint8_t i;
    uint16_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i = 0; i < 4; i++)
    {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6] ^= *(pSnr + i);
    }
    CalculateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);

    MFRC_ResetBits(Status2Reg, 0x08);

    status = MFRC_CmdFrame(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);

    if ((status == MFRC_OK) && (unLen == 0x18))
    {
        status = MFRC_OK;
    }
    else
    {
        status = MFRC_ERR;
    }

    return status;
}

/**
 * @brief  Verify cards key.
 * @param  AuthMode: Verify mode.
 *      0x60: Verify key A.
 *      0x61: Verify key B.
 * @param  Address: Address of block.
 * @param  pKey: Pointer of key.
 * @param  pSnr: Pointer of card serial number.
 * @retval Status: OK for success.
 */
char PCD_AuthState(uint8_t AuthMode, uint8_t Address, uint8_t *pKey, uint8_t *pSnr)
{
    char status;
    uint16_t unLen;
    uint8_t i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = AuthMode;
    ucComMF522Buf[1] = Address;
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }

    status = MFRC_CmdFrame(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if ((status != MFRC_OK) || (!(MFRC_ReadReg(Status2Reg) & 0x08)))
    {
        status = MFRC_ERR;
    }

    return status;
}

/**
 * @brief  Read 1 block data from card.
 * @param  Address: Address of block.
 * @param  pData: Pointer of read data.
 * @retval Status: OK for success.
 */
char PCD_Read(uint8_t addr, uint8_t *pData)
{
    char status;
    uint16_t unLen;
    uint8_t i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalculateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = MFRC_CmdFrame(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status == MFRC_OK) && (unLen == 0x90))
    {
        for (i = 0; i < 16; i++)
        {
            *(pData + i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MFRC_ERR;
    }

    return status;
}

/**
 * @brief  Write 1 block data to card.
 * @param  Address: Address of block.
 * @param  pData: Pointer of data to be wrote.
 * @retval Status: OK for success.
 */
char PCD_Write(uint8_t addr, uint8_t *pData)
{
    char status;
    uint16_t unLen;
    uint8_t i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalculateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = MFRC_CmdFrame(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MFRC_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MFRC_ERR;
    }

    if (status == MFRC_OK)
    {
        for (i = 0; i < 16; i++)
        {
            ucComMF522Buf[i] = *(pData + i);
        }
        CalculateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        status = MFRC_CmdFrame(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((status != MFRC_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MFRC_ERR;
        }
    }

    return status;
}

/**
 * @brief  Go into sleep.
 * @param  None.
 * @retval Status: OK for success.
 */
char PCD_Halt(void)
{
    uint16_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalculateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    MFRC_CmdFrame(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    return MFRC_OK;
}

/**
 * @brief  Reset device.
 * @param  None.
 * @retval Status: OK for success.
 */
char PCD_Reset(void)
{
    // Hardware reset
    GPIO_SetBits(NFC_RST_PINGROUP, NFC_RST_PIN);
    Delay_us(10);
    GPIO_ResetBits(NFC_RST_PINGROUP, NFC_RST_PIN);
    Delay_us(10);
    GPIO_SetBits(NFC_RST_PINGROUP, NFC_RST_PIN);
    Delay_us(10);

    // Software reset
    MFRC_WriteReg(CommandReg, PCD_RESETPHASE);
    while (MFRC_ReadReg(CommandReg) & 0x0F)
        ;
    Delay_us(1);

    MFRC_WriteReg(ModeReg, 0x3D);
    MFRC_WriteReg(TReloadRegL, 30);
    MFRC_WriteReg(TReloadRegH, 0);
    MFRC_WriteReg(TModeReg, 0x8D);
    MFRC_WriteReg(TPrescalerReg, 0x3E);
    MFRC_WriteReg(TxAutoReg, 0x40);

    return MFRC_OK;
}

/*===============================================================================
*************************RC522 extern operation functions************************
================================================================================*/
/**
 * @brief  Initialize all pins and SPI2.
 * @param  None.
 * @retval None.
 */
void NFC_PinInit(void)
{
    // GPIO initialize
    //  RST
    RCC_AHB1PeriphClockCmd(NFC_RST_CLOCK, ENABLE); // Enable clock
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = NFC_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NFC_RST_PINGROUP, &GPIO_InitStructure);

    // NSS
    RCC_AHB1PeriphClockCmd(NFC_NSS_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = NFC_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NFC_NSS_PINGROUP, &GPIO_InitStructure);

    // IRQ
    RCC_AHB1PeriphClockCmd(NFC_IRQ_CLOCK, ENABLE); // Enable clock
    GPIO_InitStructure.GPIO_Pin = NFC_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(NFC_IRQ_PINGROUP, &GPIO_InitStructure);

    // SCK
    RCC_AHB1PeriphClockCmd(NFC_SCK_CLOCK, ENABLE); // Enable clock
    GPIO_PinAFConfig(NFC_SCK_PINGROUP, NFC_SCK_PINSOURCE, GPIO_AF_SPI2);
    GPIO_InitStructure.GPIO_Pin = NFC_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NFC_SCK_PINGROUP, &GPIO_InitStructure);

    // MISO
    RCC_AHB1PeriphClockCmd(NFC_MISO_CLOCK, ENABLE); // Enable clock
    GPIO_PinAFConfig(NFC_MISO_PINGROUP, NFC_MISO_PINSOURCE, GPIO_AF_SPI2);
    GPIO_InitStructure.GPIO_Pin = NFC_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NFC_MISO_PINGROUP, &GPIO_InitStructure);

    // MOSI
    RCC_AHB1PeriphClockCmd(NFC_MOSI_CLOCK, ENABLE); // Enable clock
    GPIO_PinAFConfig(NFC_MOSI_PINGROUP, NFC_MOSI_PINSOURCE, GPIO_AF_SPI2);
    GPIO_InitStructure.GPIO_Pin = NFC_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NFC_MOSI_PINGROUP, &GPIO_InitStructure);

    // Initialize SPI2
    GPIO_SetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN);
    SPI_Cmd(SPI2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);
}

/**
 * @brief  Initialize and reset RC522.
 * @param  None.
 * @retval None.
 */
void NFC_Init(void)
{
    NFC_PinInit();
    GPIO_SetBits(NFC_RST_PINGROUP, NFC_NSS_PIN);
    GPIO_SetBits(NFC_NSS_PINGROUP, NFC_NSS_PIN);
    PCD_Reset();
    PCD_AntennaOff();
    Delay_us(10);
    PCD_AntennaOn();
    M500PcdConfigISOType('A');
}

/**
 * @brief  Get data from NFC card.
 * @param  pData: Pointer of data received.
 * @retval None.
 */
char NFC_GetData(uint8_t *pData)
{
    /*
    uint8_t snr,
        TagType[2],
        SelectedSnr[4],
        DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    char status = PCD_Request(REQ_ALL, TagType);
    if (!status)
    {
        //printf("Found\n");
        status = PCD_Anticoll(SelectedSnr);
        if (!status)
        {
            //printf("Anti\n");
            status = PCD_Select(SelectedSnr);
            if (!status)
            {
                //printf("Selected\n");
                snr = 1;                                                              // Sector number 1
                status = PCD_AuthState(KEYA, (snr * 4 + 3), DefaultKey, SelectedSnr); // Verify key
                {
                    if (!status)
                    {
                        status = PCD_Read((snr * 4 + 0), pData); // Read
                        // status = PcdWrite((snr*4+0), buf);  // write
                        if (!status)
                        {
                            //printf("Read\n");
                        }
                    }
                }
            }
        }
    }
    return status;
    */
    char cStr[30], tStr[30];
    uint8_t ucArray_ID[4];  //先后存放IC卡的类型和UID(IC卡序列号)
    uint8_t ucStatusReturn; //返回状态
    while (1)
    {
        //寻卡
        if ((ucStatusReturn = PCD_Request(PICC_REQALL, ucArray_ID)) != MFRC_OK)
            ///若失败再次寻卡
            ucStatusReturn = PCD_Request(PICC_REQALL, ucArray_ID);

        if (ucStatusReturn == MFRC_OK)
        {
            printf("The Card Type is: %02X%02X\n", ucArray_ID[0], ucArray_ID[1]);
            //防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
            if (PCD_Anticoll(ucArray_ID) == MFRC_OK)
            {
                printf("The Card ID is: %02X%02X%02X%02X\n", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
            }
        }
    }
}

/***********************************END OF FILE********************************/
