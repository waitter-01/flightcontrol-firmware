
#include "drvuartps.h"
#include "drvuartpl.h"
#include "uart.h"
#include "usruart.h"


UART g_UsrUartPS0;
BOOL UsrUartPS0Init(void)
{
	PUART pUart = &g_UsrUartPS0;

	pUart->Init = DrvUartPSInit;	
	pUart->Open = DrvUartPSOpen;
	pUart->Close = DrvUartPSClose;
	pUart->Read = DrvUartPSRead;
	pUart->Write = DrvUartPSWrite;	
	pUart->Param.dwBaseAddr = 0xE0000000;
	pUart->Param.dwBaud = 115200;
	pUart->Param.byDataBits = 8;
	pUart->Param.byParity = 0;

	return UartInit(pUart);
}

UART g_UsrUartPS1;
BOOL UsrUartPS1Init(void)
{
	PUART pUart = &g_UsrUartPS1;

	pUart->Init = DrvUartPSInit;	
	pUart->Open = DrvUartPSOpen;
	pUart->Close = DrvUartPSClose;
	pUart->Read = DrvUartPSRead;
	pUart->Write = DrvUartPSWrite;	
	pUart->Param.dwBaseAddr = 0xE0001000;
	pUart->Param.dwBaud = 115200;
	pUart->Param.byDataBits = 8;
	pUart->Param.byParity = 0;

	return UartInit(pUart);
}

UART g_UsrUartPL[USR_UART_PL_MAX];

BOOL UsrUartPLInit(void)
{
	DWORD dwIndex;
	PUART pUart;

	for(dwIndex = 0; dwIndex < USR_UART_PL_MAX; dwIndex++)
//	for(dwIndex = 0; dwIndex < 8; dwIndex++)
	{
		pUart = &g_UsrUartPL[dwIndex];
		pUart->Init = DrvUartPLInit;
		pUart->Open = DrvUartPLOpen;
		pUart->Close = DrvUartPLClose;
		pUart->Read = DrvUartPLRead;
		pUart->Write = DrvUartPLWrite;
		pUart->Ioctl = DrvUartPLIoctl;
		pUart->Param.dwDDRAddr = 0x1000000 + 0x1000 * (dwIndex * 2 + 1);
		pUart->Param.dwBaseAddr = 0x43C00000+(dwIndex/4)*0x10000+(dwIndex%4)*0x40;
//		pUart->Param.dwBaseAddr = 0x43C00000;
		pUart->Param.dwBaud = 115200;
		pUart->Param.byDataBits = 8;
		pUart->Param.byParity = 0;
		pUart->Param.bNoParity = FALSE;
	}

	return TRUE;
}

