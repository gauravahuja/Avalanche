#ifndef SERIAL_H
#define SERIAL_H

#include <windows.h>

enum SerialParity  { None, Odd, Even };

class Serial
{
private:
    char			Port[10];
    int				Baud;
    SerialParity	Parity;
    HANDLE			SerialHandle;

public:
	Serial();
	~Serial();
    int		Connect(char *PortName, int BaudRate, SerialParity ParityMode, DWORD TimeOut);
    int		Write(char* Data, int Length = 1);	
	int		Read(char* Data, int Length = 1);
	char*	ReadLine(char EOL = '\n'/*'\n'*/);
	int		GetNumberOfBytes(void);
    void	Disconnect(void);
	int		FlushInputBuffer();
	int		FlushOutputBuffer();
	int		Flush();
};

#endif SERIAL_H