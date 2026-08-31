/*
 * drvcanip.c
 *
 *  Created on: 2022年1月26日
 *      Author: liyuan
 */
#include "drvcanip.h"
#include "can.h"
#include "xcanps.h"
#include "usrcan.h"
#include "Basic.h"

XCanPs Can0, Can1;						//can驱动实例
XCanPs *pCanInst[CAN_DEV_MAX_NUM] = {&Can0, &Can1};
XCanPs_Config *ConfigPtr[CAN_DEV_MAX_NUM];


BOOL DrvCanInit(void *arg)
{
	int Status;

	PCAN_PARAM pParam = (PCAN_PARAM)arg;
//	DWORD BaseAddress = pParam->dwBaseAddr;
	DWORD BaudratePrescaler = pParam->BaudRatePrescaler;
	DWORD SyncJumpWidth = pParam->SyncJumpWidth;
	DWORD TimeSegment1 = pParam->dwFirstTimesegment;
	DWORD TimeSegment2 = pParam->dwSecondTimesegment;
	DWORD DeviceID  = pParam->DeviceID;

//	Initialize the Can device.

	ConfigPtr[DeviceID] = XCanPs_LookupConfig(DeviceID);
	if (pCanInst[DeviceID] == NULL) {
		return XST_FAILURE;
	}
	Status = XCanPs_CfgInitialize(pCanInst[DeviceID],
					ConfigPtr[DeviceID],
					ConfigPtr[DeviceID]->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Run self-test on the device, which verifies basic sanity of the
	 * device and the driver.
	 */
	Status = XCanPs_SelfTest(pCanInst[DeviceID]);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enter Configuration Mode so we can setup Baud Rate Prescaler
	 * Register (BRPR) and Bit Timing Register (BTR).
	 */
	XCanPs_EnterMode(pCanInst[DeviceID], XCANPS_MODE_CONFIG);
	while(XCanPs_GetMode(pCanInst[DeviceID]) != XCANPS_MODE_CONFIG);

	/*
	 * Setup Baud Rate Prescaler Register (BRPR) and
	 * Bit Timing Register (BTR).
	 */
	XCanPs_SetBaudRatePrescaler(pCanInst[DeviceID], BaudratePrescaler);
	XCanPs_SetBitTiming(pCanInst[DeviceID], SyncJumpWidth,
									TimeSegment2,
									TimeSegment1);

//	XCanPs_SetBaudRatePrescaler(pCanInst[DeviceID], TEST_BRPR_BAUD_PRESCALAR);
//	XCanPs_SetBitTiming(pCanInst[DeviceID], TEST_BTR_SYNCJUMPWIDTH,
//				TEST_BTR_SECOND_TIMESEGMENT,
//				TEST_BTR_FIRST_TIMESEGMENT);

	/*
	 * Enter normal Mode.
	 */
	XCanPs_EnterMode(pCanInst[DeviceID], XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(pCanInst[DeviceID]) != XCANPS_MODE_NORMAL);

	return 0;

}



BOOL DrvCanOpen(void *arg)
{
	//PCAN_PARAM pParam = (PCAN_PARAM)arg;

	return TRUE;
}


BOOL DrvCanClose(void *arg)
{
	//PCAN_PARAM pParam = (PCAN_PARAM)arg;

	return TRUE;
}

BOOL DrvCanIoctl(void *arg)
{
	//PCAN_PARAM pParam = (PCAN_PARAM)arg;

	return TRUE;
}


DWORD DrvCanRead(void *arg, u32 *pbyData)
{
	int Status;
	PCAN_PARAM pParam = (PCAN_PARAM)arg;
	DWORD DeviceID  = pParam->DeviceID;
	/*
	 * Wait until a frame is received.
	 */
	while (XCanPs_IsRxEmpty(pCanInst[DeviceID]) == TRUE);

	/*
	 * Receive a frame and verify its contents.
	 */
	Status = XCanPs_Recv(pCanInst[DeviceID], pbyData);

	return Status;


}

DWORD DrvCanWrite(void *arg, u8 cmd,u32 *pbyData,u8 len)
{
	int Status;

	PCAN_PARAM pParam = (PCAN_PARAM)arg;
	DWORD DeviceID  = pParam->DeviceID;
	DWORD MsgID = pParam->MessageID + cmd;

	if(((MsgID >>11) & 0x3ff) > 0 )						//Judging whether it is Extended ID
	{
//	Input:	StandardId ;SubRemoteTransReq; Identifier Extension ;ExtendedId; RemoteTransReq
		pbyData[0] = (u32)XCanPs_CreateIdValue((MsgID>>18), 0, 1, MsgID, 0);
	}
	else
	{
		pbyData[0] = (u32)XCanPs_CreateIdValue(MsgID, 0, 0, 0, 0);		// Standard ID
	}
	/*
	 * Create correct values for Identifier and Data Length Code Register.
	 */
//	pbyData[0] = (u32)XCanPs_CreateIdValue((u32)MESSAGE_ID, 0, 0, 0, 0);
	pbyData[1] = (u32)XCanPs_CreateDlcValue((u32)len);

	/*
	 * Wait until TX FIFO has room.
	 */
	while (XCanPs_IsTxFifoFull(pCanInst[DeviceID]) == TRUE);

	/*
	 * Send the frame.
	 */
	Status = XCanPs_Send(pCanInst[DeviceID], pbyData);

	return Status;
}

BOOL Get_Can_Read_Flag(u32 DeviceID)
{
	return XCanPs_IsRxEmpty(pCanInst[DeviceID]);
}

int Can_Read(u8 dir,u32 *id,u8 *data_len,u8 *buff)
{
	int Status;
	u32 tempbuff[16];

	Status = DrvCanRead(&g_UsrCan[dir],tempbuff);

//	xil_printf("Can_Read:");
//	for(u8 a=0;a<16;a++)
//	{
//		xil_printf("%x ",L2B32(*(tempbuff+a)));
//	}
//	xil_printf("\r\n");

	if(Status == XST_SUCCESS)
	{
		*id = L2B16(L2B32(tempbuff[0])) >> 5;

		if(L2B32(tempbuff[1]) & 0x0000ffff)
		{
			*data_len = (L2B32(tempbuff[1]) & 0x0000ffff) >> 4;
		}
		else
		{
			return XST_FAILURE;
		}
		memcpy(buff,tempbuff+2,8);
	}

	return Status;
}

int Can_Write(u8 dir,u8 cmdid,u8 *buff,u8 len)
{
	int Status;
	u32 tempbuff[16];

	memcpy(tempbuff+2,buff,8);
	Status = DrvCanWrite(&g_UsrCan[dir],cmdid,tempbuff,len);

	return Status;
}

