/*
 * usrcan.c
 *
 *  Created on: 2022年1月28日
 *      Author: liyuan
 */

#include "usrcan.h"
#include "can.h"
#include "drvcanip.h"
#include "xparameters.h"

#define CAN0_BASEADDR          XPAR_PS7_CAN_0_BASEADDR
#define CAN1_BASEADDR         XPAR_PS7_CAN_1_BASEADDR

DWORD CanMessageId[USR_CAN_MAX] = {0x0120, 0x0170};

//CAN g_UsrCan[USR_CAN_MAX];
CAN_PARAM g_UsrCan[USR_CAN_MAX];
enum CanBaudrate
{
	rate_1000kbps,
	rate_800kbps,
	rate_500kbps,
	rate_250kbps,
	rate_200kbps,
	rate_100kbps,
	rate_50kbps
};


////CanBaudrate  can_bit_timing_config[6];
//CAN_PARAM  can0_param[]=
//{	/* 波特率		  baseaddr		BRP	TSEG1 TSEG2 SJW		     采样点*/
//
//	{rate_1000kbps,  0xE0008000,	2,    5,  0,  0},		   /*87.5%*/
//	{rate_800kbps,   0xE0008000,	2,    7,  0,  0},         /*90.0%*/
//	{rate_500kbps,   0xE0008000,	5,    5,  0,  0},         /*85.0%*/
//	{rate_200kbps,   0xE0008000,	14,    5,  0,  0},         /*87.5%*/
//	{rate_100kbps,   0xE0008000,	29,    5,  0,  0},         /*87.5%*/
//	{rate_50kbps,  	 0xE0008000,	59,    5,  0,  0}          /*87.5%*/
//};

/***can时钟100M，几个常用波特率参数配置***/
CAN_PARAM  can_param[]=
{	/* 波特率		  baseaddr    BRP	TSEG1 TSEG2 SJW  		 采样点*/

	{rate_1000kbps, CAN0_BASEADDR,   9,    7,   0,   0},		   /*90%*/
	{rate_800kbps,  CAN0_BASEADDR,  24,    2,   0,   0},         /*80.0%*/
	{rate_500kbps,  CAN0_BASEADDR,  24,    5,   0,   0},         /*85.0%*/
	{rate_250kbps,  CAN0_BASEADDR,  49,    5,   0,   0},         /*87.5%*/
	{rate_200kbps,  CAN0_BASEADDR,  49,    7,   0,   0},         /*90%*/
	{rate_100kbps,  CAN0_BASEADDR, 124,    5,   0,   0},         /*87.5%*/
	{rate_50kbps,   CAN0_BASEADDR, 249,    5,   0,   0}          /*87.5%*/
};


BOOL UsrCanInit(void)
{
	DWORD dwIndex;
	//PCAN pCan;
	PCAN_PARAM pCan;
	for(dwIndex = 0; dwIndex < USR_CAN_MAX; dwIndex++)
	{
//		pCan = &g_UsrCan[dwIndex];
//		pCan->Init = DrvCanInit;
//		pCan->Open = DrvCanOpen;
//		pCan->Close = DrvCanClose;
//		pCan->Read = DrvCanRead;
//		pCan->Write = DrvCanWrite;

		pCan = &g_UsrCan[dwIndex];
		pCan->dwBaseAddr = XPAR_PS7_CAN_0_BASEADDR + 0x1000 * dwIndex;
		pCan->BaudRatePrescaler = 9;
		pCan->SyncJumpWidth = 0;
		pCan->dwFirstTimesegment = 7;
		pCan->dwSecondTimesegment = 0;
		pCan->DeviceID = dwIndex;
		pCan->MessageID = CanMessageId[dwIndex];

		//CanInit(pCan);
		DrvCanInit(&g_UsrCan[dwIndex]);
	}
	//CanBaudConfig(0,rate_1000kbps);
	//CanIDConfig(stCanId[0], 0);
	return 0;
}

void CanSetBaud(BYTE Dev, DWORD baud)
{
	g_UsrCan[Dev].BaudRatePrescaler = baud;
	DrvCanInit(&g_UsrCan[Dev]);
}
void CanSetTime(BYTE Dev, DWORD SyncJumpWidth,DWORD dwFirstTimesegment,DWORD dwSecondTimesegment)
{
	g_UsrCan[Dev].SyncJumpWidth = SyncJumpWidth;
	g_UsrCan[Dev].dwFirstTimesegment = dwFirstTimesegment;
	g_UsrCan[Dev].dwSecondTimesegment = dwSecondTimesegment;
	DrvCanInit(&g_UsrCan[Dev]);
}

void CanBaudConfig(BYTE Dev, enum CanBaudrate baud)
{
		g_UsrCan[Dev].BaudRatePrescaler = can_param[baud].BaudRatePrescaler;
		g_UsrCan[Dev].SyncJumpWidth = can_param[baud].SyncJumpWidth;
		g_UsrCan[Dev].dwFirstTimesegment = can_param[baud].dwFirstTimesegment;
		g_UsrCan[Dev].dwSecondTimesegment = can_param[baud].dwSecondTimesegment;
		g_UsrCan[Dev].DeviceID = Dev;
		g_UsrCan[Dev].MessageID = CanMessageId[Dev];

		DrvCanInit(&g_UsrCan[Dev]);
}


DWORD CanSendFrame(int Dev, u32 *pbyData)
{
	return DrvCanWrite(&g_UsrCan[Dev],0, pbyData,8);
}


DWORD CanRecvFrame(int Dev, u32 *pbyData)
{
	return DrvCanRead(&g_UsrCan[Dev], pbyData);
}

//配置can设备ID
void CanIDConfig(DWORD dwCanId, BYTE Dev)
{
	g_UsrCan[Dev].MessageID = dwCanId;
	DrvCanInit(&g_UsrCan[Dev]);
}





