/****************************************************************
* FileName    : fifo.h
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : https://github.com/owzq
* Version     : 0.0.1
* Created     : 2013-09-30 15:29:08
* LastChange  : 2020-05-13 21:16:59
* History     : 
*****************************************************************/

#ifndef __FIFO_H__
#define __FIFO_H__

#include "utypes.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct stFIFO
{
	BYTE *pbyData;
	DWORD dwSize;
	volatile DWORD dwIn;
	volatile DWORD dwOut;
	volatile BOOL bEnabled;
	volatile UINT64 OverflowU64;
}FIFO, *PFIFO;

BOOL FifoInit(PFIFO pFifo, DWORD dwFifoSize);
void FifoClear(PFIFO pFifo);
DWORD FifoWrite(PFIFO pFifo, const BYTE *pbyData, DWORD dwLen);
DWORD FifoRead(PFIFO pFifo, BYTE *pbyData, DWORD dwLen);
DWORD FifoGetFreeSize(const PFIFO pFifo);
DWORD FifoGetDataSize(const PFIFO pFifo);
UINT64 FifoGetOverflow(const PFIFO pFifo);
DWORD FifoWriteByte( PFIFO pFifo, const BYTE byValue);
DWORD FifoReadByte( PFIFO pFifo, BYTE *pbyValue);

#ifdef __cplusplus
}
#endif


#endif
