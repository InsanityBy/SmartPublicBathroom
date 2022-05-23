#include "stm32f4xx.h"
typedef struct
{
    uint8_t TerminalNumber;      // The number of shower terminal device
    uint32_t NFCNumber;          // The number of card number, if in use
    uint32_t UserNumber;         // The number of user, if in use
    RTC_DateTypeDef CurrentDate; // Current date
    RTC_TimeTypeDef CurrentTime; // Current time
    float WaterTemperature;      // The water temperature, if in use
    float WaterFlow;             // The flow of water used
    float AccountBalance;        // The remain money of account, if in use
    uint16_t TerminalState;      // The state of terminal
                                 // bit 8: supply water or not, bit 7: terminal error or not,
                                 // bit 6: be reserved or not, bit 5: in use or not,
                                 // bit 4: card or object left or not, bit 3: first swipe card or not
                                 // bit 2: take the card off or not, bit 1: need help or not,
                                 // bit 0: need repair or not, bit 15 ~ 9 reserved
} ShowerTerminal_InformationTypeDef;

// Get each state from terminal state
#define ShowerTerminal_WaterSupply 0x0100
#define ShowerTerminal_Error 0x0080
#define ShowerTerminal_Reserve 0x0040
#define ShowerTerminal_InUse 0x0020
#define ShowerTerminal_ObjectLeft 0x0010
#define ShowerTerminal_FirstReadCard 0x0008
#define ShowerTerminal_CardOff 0x0004
#define ShowerTerminal_Help 0x0002
#define ShowerTerminal_Repair 0x0001

typedef struct
{
    uint8_t HostNumber;          // The number of shower host device
    RTC_DateTypeDef CurrentDate; // Current date
    RTC_TimeTypeDef CurrentTime; // Current time
    float Temperature;           // The environment temperature
    float Humidity;              // The environment humidity
    float Light;                 // The environent brightness
    uint8_t TerminalState;       // The state of terminal
                                 // bit 4: turn on fan or not, bit 3: turn on light or not,
                                 // bit 2: supply water or not, bit 1: need repair or not,
                                 // bit 0: need repair or not, bit 7 ~ 5: reserved
} ShowerHost_InformationTypeDef;

// Get each state from host state
#define ShowerHost_Fan 0x10
#define ShowerHost_Light 0x08
#define ShowerHost_WaterSupply 0x04
#define ShowerHost_Repair 0x02
#define ShowerHost_Help 0x01