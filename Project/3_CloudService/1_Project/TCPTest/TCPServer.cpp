#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define ShowerTerminal_WaterSupply 0x0080
#define ShowerTerminal_Reserve 0x0040
#define ShowerTerminal_InUse 0x0020
#define ShowerTerminal_ObjectLeft 0x0010
#define ShowerTerminal_FirstReadCard 0x0008
#define ShowerTerminal_CardOff 0x0004
#define ShowerTerminal_Help 0x0002
#define ShowerTerminal_Repair 0x0001

#define ShowerHost_Fan 0x10
#define ShowerHost_Light 0x08
#define ShowerHost_WaterSupply 0x04
#define ShowerHost_Repair 0x02
#define ShowerHost_Help 0x01

const char ServerIP[] = "192.168.137.1";
const int port = 10000;
const int BufferSize = 1024;

void Uint32toInt8(unsigned int Data, unsigned char *Data0, unsigned char *Data1, unsigned char *Data2, unsigned char *Data3)
{
    *Data0 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data1 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data2 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data3 = Data & 0x000000FF;
}

unsigned int Int8toUint32(unsigned char Data0, unsigned char Data1, unsigned char Data2, unsigned char Data3)
{
    unsigned int temp = 0;
    temp = temp | Data3;
    temp = temp << 8;
    temp = temp | Data2;
    temp = temp << 8;
    temp = temp | Data1;
    temp = temp << 8;
    temp = temp | Data0;
    return temp;
}

void Int32toInt8(int Data, unsigned char *Data0, unsigned char *Data1, unsigned char *Data2, unsigned char *Data3)
{
    *Data0 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data1 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data2 = Data & 0x000000FF;
    Data = Data >> 8;
    *Data3 = Data & 0x000000FF;
}

int Int8toInt32(unsigned char Data0, unsigned char Data1, unsigned char Data2, unsigned char Data3)
{
    int temp = 0;
    temp = temp | Data3;
    temp = temp << 8;
    temp = temp | Data2;
    temp = temp << 8;
    temp = temp | Data1;
    temp = temp << 8;
    temp = temp | Data0;
    return temp;
}

int ReceivedDataTerminal_Process(char *Data, int TerminalNumber)
{
    for (int i = 0; i < TerminalNumber; i++)
    {
        int StartPosition = 16 + i * 24;
        if (Data[StartPosition] != '$')
        {
            return -1;
        }

        int TerminalSerial = 0;
        unsigned int CardID = 0;
        unsigned int UserID = 0;
        int WaterSupply = 0;
        int Reserve = 0;
        int InUse = 0;
        int ObjectLeft = 0;
        int FirstReadCard = 0;
        int CardOff = 0;
        int RepairState = 0;
        int HelpState = 0;
        float Temperature;
        float WaterFlow;
        float AccountBalance;

        TerminalSerial = Data[StartPosition + 1];
        CardID = Int8toUint32(Data[StartPosition + 2], Data[StartPosition + 3],
                              Data[StartPosition + 4], Data[StartPosition + 5]);
        UserID = Int8toUint32(Data[StartPosition + 6], Data[StartPosition + 7],
                              Data[StartPosition + 8], Data[StartPosition + 9]);
        WaterSupply = ((Data[StartPosition + 10] & ShowerTerminal_WaterSupply) == ShowerTerminal_WaterSupply) ? 1 : 0;
        Reserve = ((Data[StartPosition + 10] & ShowerTerminal_Reserve) == ShowerTerminal_Reserve) ? 1 : 0;
        InUse = ((Data[StartPosition + 10] & ShowerTerminal_InUse) == ShowerTerminal_InUse) ? 1 : 0;
        ObjectLeft = ((Data[StartPosition + 10] & ShowerTerminal_ObjectLeft) == ShowerTerminal_ObjectLeft) ? 1 : 0;
        FirstReadCard = ((Data[StartPosition + 10] & ShowerTerminal_FirstReadCard) == ShowerTerminal_FirstReadCard) ? 1 : 0;
        CardOff = ((Data[StartPosition + 10] & ShowerTerminal_CardOff) == ShowerTerminal_CardOff) ? 1 : 0;
        HelpState = ((Data[StartPosition + 10] & ShowerTerminal_Help) == ShowerTerminal_Help) ? 1 : 0;
        RepairState = ((Data[StartPosition + 10] & ShowerTerminal_Repair) == ShowerTerminal_Repair) ? 1 : 0;

        Temperature = (float)Int8toInt32(Data[StartPosition + 11], Data[StartPosition + 12],
                                         Data[StartPosition + 13], Data[StartPosition + 14]) / 100;
        WaterFlow = (float)Int8toInt32(Data[StartPosition + 15], Data[StartPosition + 16],
                                       Data[StartPosition + 17], Data[StartPosition + 18]) / 100;
        AccountBalance = (float)Int8toInt32(Data[StartPosition + 19], Data[StartPosition + 20],
                                            Data[StartPosition + 21], Data[StartPosition + 22]) / 100;
        if (Data[StartPosition + 23] != '$')
        {
            return -1;
        }
        FILE *fp;
        fp = fopen("E:\\Apache24\\htdocs\\ShowerTerminalData.txt", "w");
        if (fp == NULL)
        {
            printf("ERROR");
        }
        fprintf(fp, "TerminalSerial:%02d\n"
                    "CardID:%012d\n"
                    "UserID:%012d\n"
                    "WaterSupply:%d\n"
                    "Reserve:%d\n"
                    "InUse:%d\n"
                    "ObjectLeft:%d\n"
                    "FirstReadCard:%d\n"
                    "CardOff:%d\n"
                    "RepairState:%d\n"
                    "HelpState:%d\n"
                    "Temperature:%.2f\n"
                    "WaterFlow:%.2f\n"
                    "AccountBalance:%.2f\n",
                TerminalSerial, CardID, UserID, WaterSupply, Reserve, InUse, ObjectLeft, FirstReadCard,
                CardOff, RepairState, HelpState, Temperature, WaterFlow, AccountBalance);
        fclose(fp);
    }
    return 0;
}

int TransmitDataHost_Process(void)
{
}

int ReceivedDataHost_Process(char *Data)
{
    if (Data[0] != '$')
    {
        return -1;
    }

    int HostSerial = 0;
    int FanState = 0;
    int LightState = 0;
    int WaterSupply = 0;
    int RepairState = 0;
    int HelpState = 0;
    float Temperature = 0;
    float Humidity = 0;
    float LightIntensity = 0;
    int TerminalNumber = 0;

    HostSerial = Data[1];

    FanState = ((Data[2] & ShowerHost_Fan) == ShowerHost_Fan) ? 1 : 0;
    LightState = ((Data[2] & ShowerHost_Light) == ShowerHost_Light) ? 1 : 0;
    WaterSupply = ((Data[2] & ShowerHost_WaterSupply) == ShowerHost_WaterSupply) ? 1 : 0;
    RepairState = ((Data[2] & ShowerHost_Repair) == ShowerHost_Repair) ? 1 : 0;
    HelpState = ((Data[2] & ShowerHost_Help) == ShowerHost_Help) ? 1 : 0;

    Temperature = (float)Int8toInt32(Data[3], Data[4], Data[5], Data[6]) / 100;
    Humidity = (float)Int8toInt32(Data[7], Data[8], Data[9], Data[10]) / 100;
    LightIntensity = (float)Int8toInt32(Data[11], Data[12], Data[13], Data[14]) / 100;

    TerminalNumber = Data[15];

    FILE *fp;
    fp = fopen("E:\\Apache24\\htdocs\\ShowerHostData.txt", "w");
    if (fp == NULL)
    {
        printf("ERROR");
    }
    fprintf(fp, "HostSerial:%02d\n"
                "FanState:%d\n"
                "LightState:%d\n"
                "WaterSupply:%d\n"
                "RepairState:%d\n"
                "HelpState:%d\n"
                "Temperature:%.2f\n"
                "Humidity:%.2f\n"
                "LightIntensity:%.2f\n"
                "TerminalNumber:%d\n",
            HostSerial, FanState, LightState, WaterSupply, RepairState, HelpState,
            Temperature, Humidity, LightIntensity, TerminalNumber);
    fclose(fp);

    if (Data[16 + TerminalNumber * 24] != '$')
    {
        return -1;
    }

    if (ReceivedDataTerminal_Process(Data, TerminalNumber) == -1)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    // Initialize WSA
    WORD SocketVersion = MAKEWORD(2, 2);
    WSADATA WSAData;
    if (WSAStartup(SocketVersion, &WSAData) != 0)
    {
        return 0;
    }

    // Create socket
    SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET)
    {
        printf("Socket create error!\n");
        return 0;
    }

    // Bind IP and port
    sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(port);
    ServerAddress.sin_addr.S_un.S_addr = inet_addr(ServerIP);
    if (bind(ServerSocket, (LPSOCKADDR)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        printf("Bind error!\n");
    }

    // Start Listening...
    if (listen(ServerSocket, 5) == SOCKET_ERROR)
    {
        printf("Listen error!\n");
        return 0;
    }

    // Receive data
    SOCKET ClientSocket;
    sockaddr_in ClientAddress;
    int ClientAddressLength = sizeof(ClientAddress);
    char ReceivedData[BufferSize];

    while (true)
    {
        // Wait client and accept
        printf("Wait client...\n");
        ClientSocket = accept(ServerSocket, (SOCKADDR *)&ClientAddress, &ClientAddressLength);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("Accept error!\n");
            continue;
        }

        while (true)
        {
            // Receive data
            int ReceivedDataLength = recv(ClientSocket, ReceivedData, 255, 0);

            int status = ReceivedDataHost_Process(ReceivedData);
            printf("Status:%d\n", status);

            //发送数据
            char sendData[128];
            sprintf(sendData, "$%cA$\r\n\0", 1);
            send(ClientSocket, sendData, strlen(sendData), 0);
        }
    }
    closesocket(ClientSocket);
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}