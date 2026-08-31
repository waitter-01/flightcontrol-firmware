/*
 * can.h
 *
 *  Created on: 2022��1��26��
 *      Author: liyuan
 */

#ifndef SRC_UCAS_PLATFORM_GENERAL_CAN_CAN_H_
#define SRC_UCAS_PLATFORM_GENERAL_CAN_CAN_H_

#include "utypes.h"


typedef struct stCAN_PARAM
{
	BYTE  Baudrate;
	DWORD dwBaseAddr;
	DWORD BaudRatePrescaler;		// BRP
	DWORD dwFirstTimesegment;		// TSEG1
	DWORD dwSecondTimesegment;		// TSEG2
	DWORD SyncJumpWidth;			// SJW
	DWORD DeviceID;					// device id number
	DWORD MessageID;				// Standard or Extended Message ID

}CAN_PARAM, *PCAN_PARAM;


typedef struct stCAN
{
	BOOL (* Init)(void *);
	BOOL (* Open)(void *);
	BOOL (* Close)(void *);
	DWORD (* Read)(void *, u32 *);
	DWORD (* Write)(void *, u32 *);
	DWORD (* Ioctl)(void *);
	struct stCAN *pHandle;
	CAN_PARAM Param;

}CAN, *PCAN;


BOOL CanInit(PCAN pCan);
BOOL CanOpen(PCAN pCan);
BOOL CanClose(PCAN pCan);
DWORD CanRead(PCAN pCan, u32 *pbyData);
DWORD CanWrite(PCAN pCan, u32 *pbyData);
DWORD CanIoCtl(PCAN pCan);

void CanSetBaud(BYTE Dev, DWORD baud);
void CanSetTime(BYTE Dev, DWORD SyncJumpWidth,DWORD dwFirstTimesegment,DWORD dwSecondTimesegment);
#endif /* SRC_UCAS_PLATFORM_GENERAL_CAN_CAN_H_ */
