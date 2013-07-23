#include <windows.h>
#include "Serial.h"
#include <string.h>


Serial::Serial()
{
	Port[0]			= 0;
    Parity			= None;
    Baud			= 0;
    SerialHandle	= INVALID_HANDLE_VALUE;
}

Serial::~Serial()
{
    if (SerialHandle != INVALID_HANDLE_VALUE)
	{
        CloseHandle(SerialHandle);
	}
    SerialHandle = INVALID_HANDLE_VALUE;
}

void Serial::Disconnect(void)
{
    if (SerialHandle != INVALID_HANDLE_VALUE)
	{
        CloseHandle(SerialHandle);
	}
    SerialHandle = INVALID_HANDLE_VALUE;
}

int  Serial::Connect(char *PortName, int BaudRate, SerialParity ParityMode, DWORD TimeOut)
{
    int Error;
    DCB  dcb;
    COMMTIMEOUTS CTO = { 0, 0,	TimeOut, 0, TimeOut };

	if (SerialHandle != INVALID_HANDLE_VALUE)
	{
        CloseHandle(SerialHandle);
	}
    SerialHandle = INVALID_HANDLE_VALUE;

    Error = 0;

    if(PortName != NULL)
    {
        strncpy(Port, PortName, 10);
        Baud	= BaudRate;
        Parity	= ParityMode;
		ZeroMemory(&dcb, sizeof(dcb));
        
        dcb.DCBlength       = sizeof(dcb);                   
        dcb.BaudRate        = Baud;

        switch(Parity)
        {
            case None:
				dcb.Parity      = NOPARITY;
				dcb.fParity     = 0;
				break;
            case Even:
				dcb.Parity      = EVENPARITY;
				dcb.fParity     = 1;
				break;
            case Odd:
				dcb.Parity      = ODDPARITY;
				dcb.fParity     = 1;
				break;
        }

		dcb.StopBits        = ONESTOPBIT;
        dcb.ByteSize        = 8;
        
        dcb.fOutxCtsFlow    = false;
        dcb.fOutxDsrFlow    = false;
        dcb.fDtrControl     = DTR_CONTROL_DISABLE;
        dcb.fDsrSensitivity = 0;
        dcb.fRtsControl     = RTS_CONTROL_DISABLE;
        dcb.fOutX           = 0;
        dcb.fInX            = 0;
        
        dcb.fErrorChar      = 0;
        dcb.fBinary         = true;
        dcb.fNull           = 0;
        dcb.fAbortOnError   = 0;
        dcb.wReserved       = 0;
        dcb.XonLim          = 2;
        dcb.XoffLim         = 4;
        dcb.XonChar         = 0x13;
        dcb.XoffChar        = 0x19;
        dcb.EvtChar         = 0;
        
        SerialHandle = CreateFileA(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);

        if(SerialHandle != INVALID_HANDLE_VALUE)
        {
            if(!SetCommMask(SerialHandle, 0))
                Error = 1;
           
			// set timeouts
            if(!SetCommTimeouts(SerialHandle,&CTO))
                Error = 2;

            // set DCB
            if(!SetCommState(SerialHandle,&dcb))
                Error = 4;
        }
        else
		{
			Error = 8;
		}
    }
    else
	{
        Error = 16;
	}
	
	if(Error)
    {
        CloseHandle(SerialHandle);
        SerialHandle = INVALID_HANDLE_VALUE;
    }

	if(Error)
	{
		return 0;
	}
	else
		return 1;
}

int Serial::Write(char* Data, int Length)
{
	int BytesWritten;

    if (SerialHandle != INVALID_HANDLE_VALUE)
	{
        WriteFile(SerialHandle, Data, Length, (LPDWORD)&BytesWritten, NULL);
	}
	return BytesWritten;
}

int Serial::Read(char* Data, int Length)
{
	unsigned long ReadBytes = 0;
    if (SerialHandle != INVALID_HANDLE_VALUE)
    {
        ReadFile(SerialHandle, Data, Length, &ReadBytes, NULL);
    }
    return((int) ReadBytes);
}

char* Serial::ReadLine(char EOL)
{
	char Line[250];
	int i = 0;
	do
	{
		Read(Line + i, 1);
		i++;
	}while((Line[i-1] != EOL) && i < 249);
	if(Line[i-1] == EOL)
	{
		Line[i] = 0;
		i++;
		char *FinalLine = new char[i];
		strncpy(FinalLine, Line, i);
		return FinalLine;
	}
	return NULL;
}

int Serial::GetNumberOfBytes(void)
{
    struct _COMSTAT status;
    int             n;
    unsigned long   etat;

    n = 0;

    if (SerialHandle != INVALID_HANDLE_VALUE)
    {
        ClearCommError(SerialHandle, &etat, &status);
        n = status.cbInQue;
    }

    return(n);
}

int Serial::FlushInputBuffer()
{
	if (SerialHandle != INVALID_HANDLE_VALUE)
    {
		return PurgeComm(SerialHandle, PURGE_RXCLEAR);
	}
	return false;
}

int Serial::FlushOutputBuffer()
{
	if (SerialHandle != INVALID_HANDLE_VALUE)
    {
		return PurgeComm(SerialHandle, PURGE_TXCLEAR);
	}
	return false;
}

int Serial::Flush()
{
	if (SerialHandle != INVALID_HANDLE_VALUE)
    {
		return PurgeComm(SerialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
	}
	return false;
}