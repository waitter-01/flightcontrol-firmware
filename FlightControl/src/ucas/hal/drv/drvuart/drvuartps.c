#include "drvuartps.h"
#include "uart.h"
#include "xuartps.h"

XUartPs Uart_PS;
u8 RecvBuffer[16];	/* Buffer for Receiving Data */

#define TEST_BUFFER_SIZE 16

BOOL DrvUartPSInit(void *arg)
{
		PUART_PARAM pParam = (PUART_PARAM)arg;
}

BOOL DrvUartPSOpen(void *arg)
{
		PUART_PARAM pParam = (PUART_PARAM)arg;
}

BOOL DrvUartPSClose(void *arg)
{
		PUART_PARAM pParam = (PUART_PARAM)arg;
}

DWORD DrvUartPSRead(void *arg, BYTE *pbyData, DWORD dwSize)
{
	unsigned int ReceivedCount;

	PUART_PARAM pParam = (PUART_PARAM)arg;

	//int Status;
	XUartPs_Config *Config;
	//unsigned int SentCount;

	/*
	 * Initialize the UART driver so that it's ready to use.
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XUartPs_LookupConfig(0);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);


	ReceivedCount = 0;
		while (ReceivedCount < TEST_BUFFER_SIZE) {
			ReceivedCount +=
				XUartPs_Recv(&Uart_PS, &RecvBuffer[ReceivedCount],
						  (TEST_BUFFER_SIZE - ReceivedCount));
		}
}

DWORD DrvUartPSWrite(void *arg, BYTE *pbyData, DWORD dwSize)
{
		PUART_PARAM pParam = (PUART_PARAM)arg;
}

