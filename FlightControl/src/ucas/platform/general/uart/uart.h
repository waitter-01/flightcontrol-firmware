#ifndef __UART_H__
#define __UART_H__

#include "utypes.h"

typedef struct stUART_PARAM
{
	DWORD dwBaseAddr;
	DWORD dwBaud;
	DWORD dwDDRAddr;
	BYTE byDataBits;
	BYTE byParity;
	BOOL bNoParity;
}UART_PARAM, *PUART_PARAM;

typedef struct stUART{
	BOOL (* Init)(void *);
	BOOL (* Open)(void *);
	BOOL (* Close)(void *);
	DWORD (* Read)(void *, BYTE *, DWORD);
	DWORD (* Write)(void *, BYTE *, DWORD);
	DWORD (* Ioctl)(void *);
	struct stUART *pHandle;
	UART_PARAM Param;
}UART, *PUART;

BOOL  UartInit (PUART pUart);
BOOL  UartOpen (PUART pUart);
BOOL  UartClose (PUART pUart);
DWORD UartRead (PUART pUart, BYTE * pbyData, DWORD dwSize);
DWORD UartWrite (PUART pUart, BYTE * pbyData, DWORD dwSize);
DWORD UartIoctl(PUART pUart);


#endif
