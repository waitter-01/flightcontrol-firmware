/*
 * Basic.c
 *
 *  Created on: 2024Äê6ÔÂ17ÈÕ
 *      Author: wanghaobo
 */
#include "Basic.h"
#include "xuartps.h"
#include "usruart.h"
#include "main.h"

#include "drvuartpl.h"
#include "qflash.h"
#include "usrcan.h"
#include "voltsensor.h"
#include "usrled.h"
#include "tempsensor.h"
#include "usrtemp.h"
#include "interface.h"
#include "usrpdsxcontrol.h"
#include "RcControl.h"


static XUartPs Uart0Ps;
static UART_PARAM g_UartPL[USR_UART_PL_MAX];
static uint8_t uartpl_buf[1024 * 4 * 2 * USR_UART_PL_MAX] __attribute__((aligned(32)));
static uint16_t InitializationStatus_Lock = 0;
static bool InitializationCompleted = false;

SemaphoreHandle_t UartPsRxMessagesMutex;
SemaphoreHandle_t UartPsTxMessagesMutex;

bool getInitializationCompleted(){ return InitializationCompleted; }
void setInitializationCompleted()
{
	if(InitializationStatus_Lock == 0)
		InitializationCompleted = true;
}
void LockInitializationStatus(){ ++InitializationStatus_Lock; }
void UnLockInitializationStatus(){ --InitializationStatus_Lock; }

int UartPl_SendBuff(int ch, const unsigned char *TxBuff, unsigned short Length)
{
	int WriteLength;
	WriteLength = DrvUartPLWrite(&g_UartPL[ch], TxBuff, Length);

	return WriteLength;

}

int UartPl_RecvBuff(int ch, unsigned char * RxBuff)
{
	int dwRet = 0;

	dwRet = DrvUartPLRead(&g_UartPL[ch], RxBuff, dwRet);

	return dwRet;

}

u8 UartPl_Get_Recvflag(int ch)
{
	return Xil_In32(g_UartPL[ch].dwBaseAddr + 0x24);
}

int UartPl_Get_Recvlen(int ch)
{
	return Xil_In32(g_UartPL[ch].dwBaseAddr + 0x20);
}

uint8_t xor_checksum(const u8 *data, u32 length)
{
    u8 checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

int UART_PL_Setting(int ch, unsigned long Baud, char Par)
{
	if( Par == 'n' || Par == 'N')
	{
		g_UartPL[ch].bNoParity = FALSE;
	}
	else if(Par == 'e'|| Par == 'E')
	{
		g_UartPL[ch].bNoParity = TRUE;
		g_UartPL[ch].byParity = 0;
	}
	else
	{
		g_UartPL[ch].bNoParity = TRUE;
		g_UartPL[ch].byParity = 1;
	}

	g_UartPL[ch].dwBaud = Baud;

	DrvUartPLInit(&g_UartPL[ch]);

	return 0;
}

void UartPs_Handler(XUartPs *InstancePtr)
{
	u32 IsrStatus;
	u8 uartps_buff[64];
	u8 uartps_len = 0;
	BaseType_t xHigherPriorityTaskWoken;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Read the interrupt ID register to determine which
	 * interrupt is active
	 */
	IsrStatus = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);

	IsrStatus &= XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_ISR_OFFSET);

	/* Dispatch an appropriate handler. */
	if((IsrStatus & ((u32)XUARTPS_IXR_RXOVR | (u32)XUARTPS_IXR_RXEMPTY |
			(u32)XUARTPS_IXR_RXFULL)) != (u32)0) {
		/* Received data interrupt */
		xSemaphoreGiveFromISR(UartPsRxMessagesMutex,&xHigherPriorityTaskWoken);
//		while(XUartPs_IsReceiveData(InstancePtr->Config.BaseAddress))
//		{
//			uartps_buff[uartps_len++] = XUartPs_RecvByte(InstancePtr->Config.BaseAddress);
//			//xil_printf("readdata:%x ",uartps_buff[uartps_len-1]);
//		}
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_TOUT)) != (u32)0) {
		/* Received Timeout interrupt */
		xSemaphoreGiveFromISR(UartPsRxMessagesMutex,&xHigherPriorityTaskWoken);
//		while(XUartPs_IsReceiveData(InstancePtr->Config.BaseAddress))
//		{
//			uartps_buff[uartps_len++] = XUartPs_RecvByte(InstancePtr->Config.BaseAddress);
//			//xil_printf("outdata:%x ",uartps_buff[uartps_len-1]);
//		}
	}

	/* Clear the interrupt status. */
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_ISR_OFFSET,IsrStatus);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int UartPS0Init(void)
{
	int Status;
	XUartPs_Config *Config;
	XUartPsFormat UartPsFormat;
	u32 IntrMask;

	Config = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart0Ps, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Check hardware build */
	Status = XUartPs_SelfTest(&Uart0Ps);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XScuGic_Connect(&xInterruptController, XPS_UART0_INT_ID,
				  (Xil_ExceptionHandler) UartPs_Handler,
				  (void *) &Uart0Ps);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	//XScuGic_SetPriorityTriggerType(&xInterruptController);
	//XScuGic_InterruptMaptoCpu(&xInterruptController);
	IntrMask = XUARTPS_IXR_TOUT | XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL;
	UartPsFormat.BaudRate = 420000;
	UartPsFormat.DataBits = XUARTPS_FORMAT_8_BITS;
	UartPsFormat.StopBits = XUARTPS_FORMAT_2_STOP_BIT;
	UartPsFormat.Parity = XUARTPS_FORMAT_EVEN_PARITY;

	XUartPs_SetInterruptMask(&Uart0Ps, IntrMask);
	XUartPs_SetRecvTimeout(&Uart0Ps, 1);
	XUartPs_SetFifoThreshold(&Uart0Ps, 25);
	XUartPs_SetOperMode(&Uart0Ps, XUARTPS_OPER_MODE_NORMAL);

	XUartPs_DisableUart(&Uart0Ps);
	XUartPs_SetDataFormat(&Uart0Ps,&UartPsFormat);
	XUartPs_EnableUart(&Uart0Ps);

	UartPsTxMessagesMutex = xSemaphoreCreateMutex();
	UartPsRxMessagesMutex = xSemaphoreCreateMutex();

	/* Enable the interrupt for the device */
	XScuGic_Enable(&xInterruptController, XPS_UART0_INT_ID);

	RcControl_init(&Uart0Ps);

	return XST_SUCCESS;
}

bool Target_is_time(u32 old_time,u32 Target_time)
{
	if( (xTaskGetTickCount() - old_time) > Target_time)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void uartpl_init(void)
{
	for(uint8_t i=0;i<USR_UART_PL_MAX;i++)
	{
		g_UartPL[i].dwDDRAddr = (DWORD)(uartpl_buf + (0x1000 * (i * 2)));
		g_UartPL[i].dwBaseAddr = 0x43C00000+(i/4)*0x10000+(i%4)*0x40;
//		pUart->Param.dwBaseAddr = 0x43C00000;
		g_UartPL[i].dwBaud = 115200;
		g_UartPL[i].byDataBits = 8;
		g_UartPL[i].byParity = 0;
		g_UartPL[i].bNoParity = FALSE;

		UART_PL_Setting(i, 921600, 'n');
	}
}

