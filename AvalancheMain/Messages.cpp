#include "Messages.h"
#include "Serial.h"

Serial Port;

void SetServoAngle(char option[5])
{
	unsigned char Number = 0;
	unsigned char Decimal = 0;

	memset(option, -1, 5);
	Number = option[0] - '0';
	if(option[2] >= '0' && option[2] <='9')
	{
		Decimal = option[2] - '0';
		Decimal *= 10;
	}
	if(option[3] >= '0' && option[3] <='9')
	{
		Decimal += option[3] - '0';
	}
	SendMessageToMCU(StopTimer);
	SendMessageToMCU(SetTime);
	SendMessageToMCU(Number);
	SendMessageToMCU(Decimal);
	SendMessageToMCU(StartTimer);
}

int InitializeMessages(char* PortName, int Baud)
{
	int Connected = Port.Connect(PortName, Baud, None, 1500);
	Port.Flush();
	return Connected;
}

int SendMessageToMCU(char Message)
{
	char Temp;
	Temp = (char)TrainByte1;
	if(!Port.Write(&Temp, 1))
		return 0;
	Temp = (char)TrainByte2;
	if(!Port.Write(&Temp, 1))
		return 0;
	Temp = (char)TrainByte3;
	if(!Port.Write(&Temp, 1))
		return 0;
	if(!Port.Write(&Temp, 1))
		return 0;
	Temp = (char)StartByte;
	if(!Port.Write(&Temp, 1))
		return 0;
	if(!Port.Write(&Message, 1))
		return 0;
	Temp = (char)EndByte;
	if(!Port.Write(&Temp, 1))
		return 0;
	return 1;
}

void DeInitializeMessages()
{
	Port.Flush();
	Port.Disconnect();
}