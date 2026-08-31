
#include "uart.h"

BOOL  UartInit (PUART pUart)
{
	return pUart->Init(&pUart->Param);
}

BOOL  UartOpen (PUART pUart)
{
	return pUart->Open(&pUart->Param);
}

BOOL  UartClose (PUART pUart)
{
	return pUart->Close(&pUart->Param);
}

DWORD UartRead (PUART pUart, BYTE * pbyData, DWORD dwSize)
{
	return pUart->Read(&pUart->Param, pbyData, dwSize);
}

DWORD UartWrite (PUART pUart, BYTE * pbyData, DWORD dwSize)
{
	return pUart->Write(&pUart->Param, pbyData, dwSize);
}

DWORD UartIoctl(PUART pUart)
{
	return pUart->Ioctl(&pUart->Param);
}
