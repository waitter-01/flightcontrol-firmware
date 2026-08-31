/*
 * can.c
 *
 *  Created on: 2022Äê1ÔÂ26ÈÕ
 *      Author: liyuan
 */
#include "can.h"


BOOL CanInit(PCAN pCan)
{
	return pCan->Init(&pCan->Param);
}

BOOL CanOpen(PCAN pCan)
{
	return pCan->Open(&pCan->Param);
}

BOOL CanClose(PCAN pCan)
{
	return pCan->Close(&pCan->Param);
}

DWORD CanRead(PCAN pCan, u32 *pbyData)
{
	return pCan->Read(&pCan->Param, pbyData);
}

DWORD CanWrite(PCAN pCan, u32 *pbyData)
{
	return pCan->Write(&pCan->Param, pbyData);
}

DWORD CanIoCtl(PCAN pCan)
{
	return pCan->Ioctl(&pCan->Param);
}
